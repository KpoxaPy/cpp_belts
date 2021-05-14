import json
import argparse
import webbrowser
import tempfile

parser = argparse.ArgumentParser(description='Presenting example output')
parser.add_argument('file')
args = parser.parse_args()

data = None
with open(args.file, mode="rb") as output_file:
  data = json.load(output_file)

fp = tempfile.NamedTemporaryFile(mode="w", suffix=".html", delete=False)
fp.write("<html><body>")
for resp in data:
  if "map" in resp:
    map_fp = tempfile.NamedTemporaryFile(mode="w+b", suffix=".svg", delete=False)
    map_fp.write(resp["map"].encode("utf8"))
    map_fp.close()
    fp.write("<img src=\"file://" + map_fp.name + "\" style=\"width:100%;height:100%;\">")
fp.write("</body></html>")
fp.close()

webbrowser.open(fp.name,new=2)