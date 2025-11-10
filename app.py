from flask import Flask, render_template, request
import subprocess
import re

app = Flask(__name__)

BACKEND_EXEC = "./backend_main"  # compiled C backend binary (adjust path if needed)
TOTAL_MEMORY = 1024  # total memory in KB

def run_backend(command, args=[]):
    """Runs backend_main with given command and arguments."""
    try:
        output = subprocess.check_output([BACKEND_EXEC, command] + list(map(str, args)))
        return output.decode('utf-8').strip()
    except subprocess.CalledProcessError as e:
        return f"Error: {e.output.decode('utf-8').strip()}"
    except FileNotFoundError:
        return "❌ Backend executable not found. Please compile backend_main.c first."

def parse_memory_map(text):
    """
    Parses the backend memory map output into structured data.
    Expected line format example:
    PID: 1 | Start: 0 | Size: 200 | Status: Allocated
    """
    blocks = []
    for line in text.splitlines():
        m = re.search(r"PID:\s*(\S+).*?Size:\s*(\d+).*?Status:\s*(\w+)", line)
        if m:
            blocks.append({
                "pid": m.group(1),
                "size": int(m.group(2)),
                "status": m.group(3)
            })
    return blocks

@app.route("/", methods=["GET", "POST"])
def index():
    outputs = {}
    parsed = []
    base = TOTAL_MEMORY

    if request.method == "POST":
        if "allocate_submit" in request.form:
            size = request.form.get("size")
            outputs["allocate"] = run_backend("allocate", [size])

        elif "deallocate_submit" in request.form:
            pid = request.form.get("pid")
            outputs["deallocate"] = run_backend("deallocate", [pid])

        elif "compact_submit" in request.form:
            outputs["compact"] = run_backend("compact")

        elif "relation_submit" in request.form:
            from_pid = request.form.get("from_pid")
            to_pid = request.form.get("to_pid")
            outputs["relation"] = run_backend("relation", [from_pid, to_pid])

        elif "graph_submit" in request.form:
            outputs["graph"] = run_backend("graph")

        elif "view_submit" in request.form:
            view_output = run_backend("view")
            outputs["view"] = view_output
            parsed = parse_memory_map(view_output)

    return render_template("index.html", outputs=outputs, parsed=parsed, base=base)

if __name__ == "__main__":
    app.run(debug=True)
