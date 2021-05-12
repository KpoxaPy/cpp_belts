build\main.exe make_base < find_make_base.in.json
build\main-ref.exe process_requests < find_process_requests.in.json | jq ".[].companies |= sort" > find_output_ref.json
build\main.exe process_requests < find_process_requests.in.json | jq ".[].companies |= sort" > find_output.json
jd find_output.json find_output_ref.json

jq "[.[] | [{id: .request_id, count:.companies | length}]]" find_output_ref.json > find_output_ref_counted.json
jq "[.[] | [{id: .request_id, count:.companies | length}]]" find_output.json > find_output_counted.json
jd find_output_counted.json find_output_ref_counted.json
