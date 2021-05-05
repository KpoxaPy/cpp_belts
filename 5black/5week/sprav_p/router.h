#pragma once

#include "graph.h"
#include "profile.h"
#include "transport_catalog.pb.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Graph {

  template <typename Weight, typename Extra>
  class Router {
  public:
    using Graph = DirectedWeightedGraph<Weight, Extra>;
    using Edge = Edge<Weight, Extra>;
    using RouteId = uint64_t;

    Router(const Graph& graph);
    Router(const Graph& graph, const SpravSerialize::Router& m);

    struct RouteInfo {
      RouteId id;
      Weight weight;
      size_t edge_count;
    };

    std::optional<RouteInfo> BuildRoute(VertexId from, VertexId to) const;
    EdgeId GetRouteEdgeId(RouteId route_id, size_t edge_idx) const;
    const Edge& GetRouteEdge(RouteId route_id, size_t edge_idx) const;
    void ReleaseRoute(RouteId route_id);

    void Serialize(SpravSerialize::Router& m);

  private:
    const Graph& graph_;

    struct RouteInternalData {
      Weight weight;
      std::optional<EdgeId> prev_edge;
    };
    using RoutesInternalData = std::vector<std::vector<std::optional<RouteInternalData>>>;
    RoutesInternalData routes_internal_data_;

    using ExpandedRoute = std::vector<EdgeId>;
    mutable RouteId next_route_id_ = 0;
    mutable std::unordered_map<RouteId, ExpandedRoute> expanded_routes_cache_;

    void ParseFrom(const SpravSerialize::Router& m);

    void InitializeRoutesInternalData(const Graph& graph) {
      LOG_DURATION("Router: init internal");
      const size_t vertex_count = graph.GetVertexCount();
      for (VertexId vertex = 0; vertex < vertex_count; ++vertex) {
        routes_internal_data_[vertex][vertex] = RouteInternalData{0, std::nullopt};
        for (const EdgeId edge_id : graph.GetIncidentEdges(vertex)) {
          const auto& edge = graph.GetEdge(edge_id);
          assert(edge.weight >= 0);
          auto& route_internal_data = routes_internal_data_[vertex][edge.to];
          if (!route_internal_data || route_internal_data->weight > edge.weight) {
            route_internal_data = RouteInternalData{edge.weight, edge_id};
          }
        }
      }
    }

    void RelaxRoute(VertexId vertex_from, VertexId vertex_to,
                    const RouteInternalData& route_from, const RouteInternalData& route_to) {
      auto& route_relaxing = routes_internal_data_[vertex_from][vertex_to];
      const Weight candidate_weight = route_from.weight + route_to.weight;
      if (!route_relaxing || candidate_weight < route_relaxing->weight) {
        route_relaxing = {
            candidate_weight,
            route_to.prev_edge
                ? route_to.prev_edge
                : route_from.prev_edge
        };
      }
    }

    void RelaxRoutesInternalDataThroughVertex(size_t vertex_count, VertexId vertex_through) {
      for (VertexId vertex_from = 0; vertex_from < vertex_count; ++vertex_from) {
        if (const auto& route_from = routes_internal_data_[vertex_from][vertex_through]) {
          for (VertexId vertex_to = 0; vertex_to < vertex_count; ++vertex_to) {
            if (const auto& route_to = routes_internal_data_[vertex_through][vertex_to]) {
              RelaxRoute(vertex_from, vertex_to, *route_from, *route_to);
            }
          }
        }
      }
    }
  };


  template <typename Weight, typename Extra>
  Router<Weight, Extra>::Router(const Graph& graph)
      : graph_(graph)
      , routes_internal_data_(graph.GetVertexCount(), std::vector<std::optional<RouteInternalData>>(graph.GetVertexCount()))
  {
    InitializeRoutesInternalData(graph);

    LOG_DURATION("Router: relax routes");
    const size_t vertex_count = graph.GetVertexCount();
    for (VertexId vertex_through = 0; vertex_through < vertex_count; ++vertex_through) {
      RelaxRoutesInternalDataThroughVertex(vertex_count, vertex_through);
    }
  }

  template <typename Weight, typename Extra>
  Router<Weight, Extra>::Router(const Graph& graph, const SpravSerialize::Router& m)
    : graph_(graph)
  {
    ParseFrom(m);
  }

  template <typename Weight, typename Extra>
  std::optional<typename Router<Weight, Extra>::RouteInfo> Router<Weight, Extra>::BuildRoute(VertexId from, VertexId to) const {
    const auto& route_internal_data = routes_internal_data_[from][to];
    if (!route_internal_data) {
      return std::nullopt;
    }
    const Weight weight = route_internal_data->weight;
    std::vector<EdgeId> edges;
    for (std::optional<EdgeId> edge_id = route_internal_data->prev_edge;
         edge_id;
         edge_id = routes_internal_data_[from][graph_.GetEdge(*edge_id).from]->prev_edge) {
      edges.push_back(*edge_id);
    }
    std::reverse(std::begin(edges), std::end(edges));

    const RouteId route_id = next_route_id_++;
    const size_t route_edge_count = edges.size();
    expanded_routes_cache_[route_id] = std::move(edges);
    return RouteInfo{route_id, weight, route_edge_count};
  }

  template <typename Weight, typename Extra>
  EdgeId Router<Weight, Extra>::GetRouteEdgeId(RouteId route_id, size_t edge_idx) const {
    return expanded_routes_cache_.at(route_id)[edge_idx];
  }

  template <typename Weight, typename Extra>
  const typename Router<Weight, Extra>::Edge& Router<Weight, Extra>::GetRouteEdge(RouteId route_id, size_t edge_idx) const {
    return graph_.GetEdge(expanded_routes_cache_.at(route_id)[edge_idx]);
  }

  template <typename Weight, typename Extra>
  void Router<Weight, Extra>::ReleaseRoute(RouteId route_id) {
    expanded_routes_cache_.erase(route_id);
  }

  template <typename Weight, typename Extra>
  void Router<Weight, Extra>::Serialize(SpravSerialize::Router& m) {
    for (const auto& row : routes_internal_data_) {
      auto& m_row = *m.add_row();
      for (const auto& data_opt : row) {
        auto& m_data_opt = *m_row.add_data();
        if (data_opt.has_value()) {
          auto& m_data = *m_data_opt.mutable_data();
          m_data.set_weight(data_opt->weight);
          if (data_opt->prev_edge.has_value()) {
            m_data.set_prev_edge(data_opt->prev_edge.value());
          }
        }
      }
    }
  }

  template <typename Weight, typename Extra>
  void Router<Weight, Extra>::ParseFrom(const SpravSerialize::Router& m) {
    LOG_DURATION("Router parsefrom");
    routes_internal_data_.reserve(m.row().size());
    for (const auto& m_row : m.row()) {
      std::vector<std::optional<RouteInternalData>> row;
      row.reserve(m_row.data().size());
      for (const auto& m_data_opt : m_row.data()) {
        std::optional<RouteInternalData> data_opt;
        if (m_data_opt.has_data()) {
          RouteInternalData data;
          data.weight = m_data_opt.data().weight();

          if (m_data_opt.data().optional_prev_edge_case()
            != SpravSerialize::Router::Data::OPTIONAL_PREV_EDGE_NOT_SET)
          {
            data.prev_edge = m_data_opt.data().prev_edge();
          }

          data_opt = std::move(data);
        }
        row.push_back(std::move(data_opt));
      }
      routes_internal_data_.push_back(std::move(row));
    }
  }

}
