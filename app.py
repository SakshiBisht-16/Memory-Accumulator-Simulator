from flask import Flask, render_template, request, redirect, url_for
import subprocess
import os

app = Flask(__name__)
PROJECT_PATH = "/Users/sakshibisht/Documents/project"
BACKEND = os.path.join(PROJECT_PATH, "backend_main")


def run_cmd(args):
    try:
        result = subprocess.run(
            [BACKEND] + args,
            cwd=PROJECT_PATH,
            capture_output=True,
            text=True
        )
        return result.stdout if result.stdout else ""
    except Exception as e:
        return f"Error: {str(e)}"


def parse_memory_map(text):
    rows = []
    lines = text.strip().split("\n")

    for line in lines:
        if "PID" in line or "---" in line or line.strip() == "":
            continue

        parts = line.split()
        if len(parts) < 5:
            continue

        pid, start, end, size, status = parts
        rows.append({
            "pid": pid.strip(),
            "start": start.strip(),
            "end": end.strip(),
            "size": size.strip(),
            "status": status.strip()
        })

    return rows


@app.route("/", methods=["GET", "POST"])
def dashboard():
    outputs = {}

    if request.method == "POST":
        if "allocate_submit" in request.form:
            size = request.form.get("size")
            outputs["allocate"] = run_cmd(["allocate", size])

        elif "deallocate_submit" in request.form:
            pid = request.form.get("pid")
            outputs["deallocate"] = run_cmd(["deallocate", pid])

        elif "compact_submit" in request.form:
            outputs["compact"] = run_cmd(["compact"])

        elif "relation_submit" in request.form:
            from_pid = request.form.get("from_pid")
            to_pid = request.form.get("to_pid")
            outputs["relation"] = run_cmd(["add_relation", from_pid, to_pid])

        elif "graph_submit" in request.form:
            outputs["graph"] = run_cmd(["show_graph"])

        elif "view_submit" in request.form:
            raw = run_cmd(["display"])
            outputs["display"] = parse_memory_map(raw)

    return render_template("index.html", outputs=outputs)


@app.route("/logout_action", methods=["POST"])
def logout_action():
    run_cmd(["logout"])
    return redirect(url_for("dashboard"))


if __name__ == "__main__":
    app.run(debug=True)

