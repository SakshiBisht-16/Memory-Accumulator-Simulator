from flask import Flask, render_template, request, redirect, url_for, session
import subprocess
import os

app = Flask(__name__)
app.secret_key = os.urandom(24)

BACKEND = "./backend_main"

def run_backend(cmd_args):
    """Run backend_main with args and return stdout/stderr."""
    try:
        proc = subprocess.run([BACKEND] + cmd_args, capture_output=True, text=True)
        out = proc.stdout.strip()
        err = proc.stderr.strip()
        return out + ("\n" + err if err else "")
    except Exception as e:
        return f"Backend invocation error: {e}"

@app.route("/", methods=["GET", "POST"])
def index():
    outputs = {}
    if request.method == "POST":
        if 'allocate_submit' in request.form:
            size = request.form.get("size", "0")
            outputs['allocate'] = run_backend(["allocate", size])
        elif 'deallocate_submit' in request.form:
            pid = request.form.get("pid", "")
            outputs['deallocate'] = run_backend(["deallocate", pid])
        elif 'compact_submit' in request.form:
            outputs['compact'] = run_backend(["compact"])
        elif 'relation_submit' in request.form:
            from_pid = request.form.get("from_pid", "")
            to_pid = request.form.get("to_pid", "")
            outputs['relation'] = run_backend(["add_relation", from_pid, to_pid])
        elif 'graph_submit' in request.form:
            outputs['graph'] = run_backend(["show_graph"])
        elif 'view_submit' in request.form:
            outputs['display'] = run_backend(["display"])
        elif 'logout_button' in request.form:
            # explicit logout button
            run_backend(["logout"])
            session.clear()
            return redirect(url_for('index'))

    # Optionally display the current memory map on page load
    current = run_backend(["display"])
    outputs.setdefault('current', current)

    return render_template("index.html", outputs=outputs)

@app.route("/logout", methods=["POST"])
def logout():
    """
    Called by sendBeacon() on page unload OR by explicit logout POST.
    Must be fast and return quickly.
    """
    # call backend to clear saved files
    run_backend(["logout"])
    session.clear()
    # send minimal response
    return ("", 200)

if __name__ == "__main__":
    app.run(debug=True)
