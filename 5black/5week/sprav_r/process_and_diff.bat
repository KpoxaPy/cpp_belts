build\main.exe make_base < find_make_base.in.json
build\main-ref.exe process_requests < find_process_requests.in.json | jq ".[].companies |= sort" > phones_output_ref.json
build\main.exe process_requests < find_process_requests.in.json | jq ".[].companies |= sort" > phones_output.json
jd phones_output.json phones_output_ref.json
