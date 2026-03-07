#!/usr/bin/env python3

import sys
import os
import re
import json
import math
import http.server
from collections import defaultdict
from xml.sax.saxutils import escape

PORT = 8080

BG_COLOR = "#1e1e2e"
WIRE_COLOR = "#89b4fa"
COMP_BG = "#313244"
COMP_TEXT = "#cdd6f4"
PIN_INPUT = "#a6e3a1"
PIN_OUTPUT = "#f38ba8"
PIN_TEXT = "#1e1e2e"
INPUT_COLOR = "#a6e3a1"
OUTPUT_COLOR = "#f38ba8"
CLOCK_COLOR = "#f9e2af"
SPECIAL_COLOR = "#cba6f7"
GATE_COLOR = "#89b4fa"
CHIP_COLOR = "#fab387"

COMPONENT_COLORS = {
    "input": INPUT_COLOR,
    "output": OUTPUT_COLOR,
    "clock": CLOCK_COLOR,
    "true": INPUT_COLOR,
    "false": INPUT_COLOR,
    "logger": SPECIAL_COLOR,
}

CHIP_PIN_COUNTS = {
    "input": 1, "output": 1, "clock": 1, "true": 1, "false": 1,
    "and": 3, "or": 3, "xor": 3, "not": 2, "nand": 3, "nor": 3,
    "4001": 14, "4008": 16, "4011": 14, "4013": 14, "4017": 16,
    "4030": 14, "4040": 16, "4069": 14, "4071": 14, "4081": 14,
    "4094": 16, "4512": 16, "4514": 24, "4801": 24, "2716": 24,
    "logger": 10,
}

INPUT_TYPES = {"input", "clock", "true", "false"}


class NTSComponent:
    def __init__(self, ctype: str, name: str):
        self.ctype = ctype
        self.name = name
        self.pins: dict[int, list[tuple[str, int]]] = defaultdict(list)


class NTSCircuit:
    def __init__(self):
        self.components: dict[str, NTSComponent] = {}
        self.links: list[tuple[str, int, str, int]] = []

    @staticmethod
    def parse_text(text: str) -> "NTSCircuit":
        circuit = NTSCircuit()
        section = None
        for raw_line in text.splitlines():
            line = raw_line.split("#")[0].strip()
            if not line:
                continue
            if line == ".chipsets:":
                section = "chipsets"
                continue
            if line == ".links:":
                section = "links"
                continue
            if section == "chipsets":
                parts = line.split()
                if len(parts) >= 2:
                    ctype, name = parts[0], parts[1]
                    circuit.components[name] = NTSComponent(ctype, name)
            elif section == "links":
                parts = line.split()
                if len(parts) >= 2:
                    m1 = re.match(r"^(.+):(\d+)$", parts[0])
                    m2 = re.match(r"^(.+):(\d+)$", parts[1])
                    if m1 and m2:
                        n1, p1 = m1.group(1), int(m1.group(2))
                        n2, p2 = m2.group(1), int(m2.group(2))
                        circuit.links.append((n1, p1, n2, p2))
                        if n1 in circuit.components:
                            circuit.components[n1].pins[p1].append((n2, p2))
                        if n2 in circuit.components:
                            circuit.components[n2].pins[p2].append((n1, p1))
        return circuit


def get_pin_count(ctype: str) -> int:
    return CHIP_PIN_COUNTS.get(ctype, 4)


def get_component_color(ctype: str) -> str:
    if ctype in COMPONENT_COLORS:
        return COMPONENT_COLORS[ctype]
    if ctype in ("and", "or", "xor", "not", "nand", "nor"):
        return GATE_COLOR
    return CHIP_COLOR


def compute_layout(circuit: NTSCircuit) -> dict[str, tuple[float, float]]:
    components = circuit.components
    if not components:
        return {}

    inputs, outputs, middle = [], [], []
    for name, comp in components.items():
        if comp.ctype in INPUT_TYPES:
            inputs.append(name)
        elif comp.ctype == "output":
            outputs.append(name)
        else:
            middle.append(name)

    adj: dict[str, set[str]] = defaultdict(set)
    for n1, _, n2, _ in circuit.links:
        adj[n1].add(n2)
        adj[n2].add(n1)

    layers: dict[str, int] = {}
    queue = []
    for name in inputs:
        layers[name] = 0
        queue.append(name)

    idx = 0
    while idx < len(queue):
        cur = queue[idx]
        idx += 1
        for nb in adj[cur]:
            if nb not in layers:
                layers[nb] = layers[cur] + 1
                queue.append(nb)

    for name in components:
        if name not in layers:
            layers[name] = 1

    max_layer = max(layers.values()) if layers else 0
    for name in outputs:
        layers[name] = max_layer + 1

    layer_groups: dict[int, list[str]] = defaultdict(list)
    for name, layer in layers.items():
        layer_groups[layer].append(name)

    positions: dict[str, tuple[float, float]] = {}
    x_spacing = 250
    y_spacing = 130

    for layer_idx in sorted(layer_groups.keys()):
        group = sorted(layer_groups[layer_idx])
        x = layer_idx * x_spacing + 120
        for i, name in enumerate(group):
            pin_count = get_pin_count(components[name].ctype)
            height_factor = max(1.0, pin_count / 4)
            y = 100 + i * y_spacing * height_factor
            positions[name] = (x, y)

    return positions


COMP_W = 150
COMP_MIN_H = 60
PIN_R = 7
CORNER_R = 8


def comp_height(comp: NTSComponent) -> float:
    pin_count = get_pin_count(comp.ctype)
    used = max(len(comp.pins), pin_count)
    half = math.ceil(used / 2)
    return max(COMP_MIN_H, half * 24 + 24)


def compute_pin_positions(circuit: NTSCircuit,
                          comp_boxes: dict) -> dict[tuple[str, int], tuple[float, float]]:
    pin_coords: dict[tuple[str, int], tuple[float, float]] = {}

    for name, comp in circuit.components.items():
        box = comp_boxes[name]
        x1, y1, x2, y2 = box
        pin_count = get_pin_count(comp.ctype)
        is_input_type = comp.ctype in INPUT_TYPES
        is_output_type = comp.ctype == "output"

        if pin_count == 1:
            if is_input_type:
                pin_coords[(name, 1)] = (x2, (y1 + y2) / 2)
            else:
                pin_coords[(name, 1)] = (x1, (y1 + y2) / 2)
        elif pin_count == 2:
            pin_coords[(name, 1)] = (x1, (y1 + y2) / 2)
            pin_coords[(name, 2)] = (x2, (y1 + y2) / 2)
        elif pin_count == 3:
            mid = (y1 + y2) / 2
            pin_coords[(name, 1)] = (x1, mid - 14)
            pin_coords[(name, 2)] = (x1, mid + 14)
            pin_coords[(name, 3)] = (x2, mid)
        else:
            half = pin_count // 2
            spacing = (y2 - y1 - 16) / max(half, 1)
            for i in range(half):
                py = y1 + 8 + spacing * (i + 0.5)
                pin_coords[(name, i + 1)] = (x1, py)
            for i in range(half):
                py = y1 + 8 + spacing * (i + 0.5)
                pin_coords[(name, pin_count - i)] = (x2, py)

    return pin_coords


def generate_svg(circuit: NTSCircuit, title: str = "") -> str:
    positions = compute_layout(circuit)

    comp_boxes: dict[str, tuple[float, float, float, float]] = {}
    for name, (cx, cy) in positions.items():
        comp = circuit.components[name]
        w = COMP_W
        h = comp_height(comp)
        comp_boxes[name] = (cx - w / 2, cy - h / 2, cx + w / 2, cy + h / 2)

    pin_coords = compute_pin_positions(circuit, comp_boxes)

    all_x = [v for name in comp_boxes for v in (comp_boxes[name][0], comp_boxes[name][2])]
    all_y = [v for name in comp_boxes for v in (comp_boxes[name][1], comp_boxes[name][3])]
    if not all_x:
        return '<svg xmlns="http://www.w3.org/2000/svg" width="400" height="100"><text x="20" y="50" fill="#cdd6f4" font-family="monospace">No components found</text></svg>'
    pad = 60
    min_x = min(all_x) - pad
    min_y = min(all_y) - pad
    max_x = max(all_x) + pad
    max_y = max(all_y) + pad
    vw = max_x - min_x
    vh = max_y - min_y

    parts: list[str] = []
    parts.append(f'<svg xmlns="http://www.w3.org/2000/svg" '
                 f'viewBox="{min_x:.0f} {min_y:.0f} {vw:.0f} {vh:.0f}" '
                 f'width="100%" height="100%" preserveAspectRatio="xMidYMid meet">')

    parts.append(f"""<style>
  svg {{ font-family: 'Courier New', monospace; }}
  .wire {{ stroke: {WIRE_COLOR}; stroke-width: 2; fill: none; stroke-linecap: round; }}
  .wire:hover {{ stroke: #f9e2af; stroke-width: 3; }}
  .comp-shadow {{ fill: #11111b; }}
  .comp-body {{ fill: {COMP_BG}; stroke-width: 2; }}
  .pin-in {{ fill: {PIN_INPUT}; stroke: {COMP_BG}; stroke-width: 1; }}
  .pin-out {{ fill: {PIN_OUTPUT}; stroke: {COMP_BG}; stroke-width: 1; }}
  .pin-label {{ fill: {PIN_TEXT}; font-size: 8px; font-weight: bold; text-anchor: middle; dominant-baseline: central; }}
  .comp-type {{ font-size: 10px; font-weight: bold; text-anchor: middle; }}
  .comp-name {{ fill: {COMP_TEXT}; font-size: 12px; font-weight: bold; text-anchor: middle; dominant-baseline: central; }}
  .legend-text {{ fill: {COMP_TEXT}; font-size: 10px; dominant-baseline: central; }}
</style>""")

    parts.append(f'<rect x="{min_x}" y="{min_y}" width="{vw}" height="{vh}" fill="{BG_COLOR}"/>')

    grid_step = 40
    gx = int(min_x // grid_step) * grid_step
    parts.append('<g opacity="0.3">')
    x = gx
    while x <= max_x:
        parts.append(f'<line x1="{x}" y1="{min_y}" x2="{x}" y2="{max_y}" stroke="#2a2a3e" stroke-width="0.5"/>')
        x += grid_step
    y = int(min_y // grid_step) * grid_step
    while y <= max_y:
        parts.append(f'<line x1="{min_x}" y1="{y}" x2="{max_x}" y2="{y}" stroke="#2a2a3e" stroke-width="0.5"/>')
        y += grid_step
    parts.append('</g>')

    if title:
        parts.append(f'<text x="{min_x + 15}" y="{min_y + 22}" '
                     f'fill="#b4befe" font-size="14" font-weight="bold">'
                     f'{escape(title)}</text>')

    parts.append('<g class="wires">')
    for n1, p1, n2, p2 in circuit.links:
        k1 = (n1, p1)
        k2 = (n2, p2)
        if k1 in pin_coords and k2 in pin_coords:
            ax, ay = pin_coords[k1]
            bx, by = pin_coords[k2]
            mx = (ax + bx) / 2
            parts.append(
                f'<path class="wire" d="M{ax:.1f},{ay:.1f} '
                f'C{mx:.1f},{ay:.1f} {mx:.1f},{by:.1f} {bx:.1f},{by:.1f}"/>'
            )
    parts.append('</g>')

    parts.append('<g class="components">')
    for name, comp in circuit.components.items():
        bx1, by1, bx2, by2 = comp_boxes[name]
        bw = bx2 - bx1
        bh = by2 - by1
        color = get_component_color(comp.ctype)

        parts.append(f'<g>')
        parts.append(f'<title>{escape(comp.ctype)} : {escape(comp.name)}</title>')

        parts.append(f'<rect class="comp-shadow" x="{bx1+3:.1f}" y="{by1+3:.1f}" '
                     f'width="{bw:.1f}" height="{bh:.1f}" rx="{CORNER_R}"/>')
        parts.append(f'<rect class="comp-body" x="{bx1:.1f}" y="{by1:.1f}" '
                     f'width="{bw:.1f}" height="{bh:.1f}" rx="{CORNER_R}" stroke="{color}"/>')

        parts.append(f'<text class="comp-type" x="{(bx1+bx2)/2:.1f}" y="{by1+15:.1f}" '
                     f'fill="{color}">{escape(comp.ctype.upper())}</text>')
        parts.append(f'<text class="comp-name" x="{(bx1+bx2)/2:.1f}" y="{(by1+by2)/2:.1f}">'
                     f'{escape(comp.name)}</text>')

        for (cname, pin_num), (px, py) in pin_coords.items():
            if cname != name:
                continue
            is_left = abs(px - bx1) < abs(px - bx2)
            pin_class = "pin-in" if is_left else "pin-out"

            parts.append(f'<circle class="{pin_class}" cx="{px:.1f}" cy="{py:.1f}" r="{PIN_R}"/>')
            parts.append(f'<text class="pin-label" x="{px:.1f}" y="{py:.1f}">{pin_num}</text>')

            if is_left:
                parts.append(f'<text x="{px+13:.1f}" y="{py:.1f}" fill="{color}" '
                             f'font-size="8" dominant-baseline="central">{pin_num}</text>')
            else:
                parts.append(f'<text x="{px-13:.1f}" y="{py:.1f}" fill="{color}" '
                             f'font-size="8" text-anchor="end" dominant-baseline="central">{pin_num}</text>')

        parts.append('</g>')
    parts.append('</g>')

    legend_items = [
        ("Input / Clock", INPUT_COLOR),
        ("Output", OUTPUT_COLOR),
        ("Logic Gate", GATE_COLOR),
        ("Chip", CHIP_COLOR),
        ("Special", SPECIAL_COLOR),
    ]
    lx = min_x + 15
    ly = max_y - len(legend_items) * 22 - 10
    parts.append('<g class="legend">')
    for label, color in legend_items:
        parts.append(f'<rect x="{lx}" y="{ly-6}" width="14" height="14" '
                     f'fill="{color}" rx="2"/>')
        parts.append(f'<text class="legend-text" x="{lx+20}" y="{ly+1}">'
                     f'{escape(label)}</text>')
        ly += 22
    parts.append('</g>')

    parts.append('</svg>')
    return "\n".join(parts)


HTML_PAGE = r"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>NanoTekSpice Visualizer</title>
<style>
  * { margin: 0; padding: 0; box-sizing: border-box; }
  body { background: #1e1e2e; color: #cdd6f4; font-family: 'Courier New', monospace; height: 100vh; display: flex; flex-direction: column; }
  header { background: #181825; padding: 10px 20px; display: flex; align-items: center; gap: 16px; border-bottom: 1px solid #313244; }
  header h1 { font-size: 16px; color: #b4befe; white-space: nowrap; }
  header .info { font-size: 12px; color: #6c7086; flex: 1; text-align: right; }
  .container { display: flex; flex: 1; overflow: hidden; }
  .editor-panel { width: 420px; min-width: 300px; display: flex; flex-direction: column; border-right: 1px solid #313244; background: #181825; }
  .editor-toolbar { padding: 8px 12px; display: flex; gap: 8px; align-items: center; border-bottom: 1px solid #313244; flex-wrap: wrap; }
  .editor-toolbar label { font-size: 12px; color: #a6adc8; cursor: pointer; }
  .editor-toolbar label:hover { color: #cdd6f4; }
  .btn { background: #313244; color: #cdd6f4; border: none; padding: 6px 14px; border-radius: 4px; cursor: pointer; font-family: inherit; font-size: 12px; }
  .btn:hover { background: #45475a; }
  .btn-primary { background: #89b4fa; color: #1e1e2e; }
  .btn-primary:hover { background: #74c7ec; }
  textarea { flex: 1; background: #1e1e2e; color: #cdd6f4; border: none; padding: 12px; font-family: 'Courier New', monospace; font-size: 13px; resize: none; outline: none; line-height: 1.5; tab-size: 2; }
  textarea::placeholder { color: #585b70; }
  .svg-panel { flex: 1; display: flex; align-items: center; justify-content: center; overflow: auto; background: #1e1e2e; position: relative; }
  .svg-panel svg { max-width: 100%; max-height: 100%; }
  .svg-panel .empty { color: #585b70; font-size: 14px; text-align: center; padding: 40px; }
  .status { background: #181825; padding: 4px 16px; font-size: 11px; color: #6c7086; border-top: 1px solid #313244; display: flex; justify-content: space-between; }
  .error-msg { color: #f38ba8; font-size: 12px; padding: 8px 12px; background: #1e1e2e; border-top: 1px solid #45475a; display: none; max-height: 60px; overflow: auto; }
  .file-input { display: none; }
  .examples { padding: 8px 12px; border-bottom: 1px solid #313244; }
  .examples select { background: #313244; color: #cdd6f4; border: 1px solid #45475a; padding: 4px 8px; border-radius: 4px; font-family: inherit; font-size: 12px; cursor: pointer; width: 100%; }
</style>
</head>
<body>
<header>
  <h1>NanoTekSpice Visualizer</h1>
  <div class="info" id="info">Edit or load a .nts file, then click Generate</div>
</header>
<div class="container">
  <div class="editor-panel">
    <div class="editor-toolbar">
      <button class="btn btn-primary" onclick="generate()" title="Ctrl+Enter">Generate</button>
      <button class="btn" onclick="document.getElementById('fileInput').click()">Load file</button>
      <button class="btn" onclick="downloadSvg()">Download SVG</button>
      <input type="file" id="fileInput" class="file-input" accept=".nts" onchange="loadFile(event)">
    </div>
    <div class="examples">
      <select id="exampleSelect" onchange="loadExample(this.value)">
        <option value="">-- Load an example --</option>
        <option value="and">AND gate</option>
        <option value="or">OR gate</option>
        <option value="not">NOT gate</option>
        <option value="xor">XOR gate</option>
        <option value="and-or-not">AND-OR-NOT combo</option>
        <option value="4008_adder">4008 Adder</option>
        <option value="4071_or">4071 OR chip</option>
        <option value="input_output">Input/Output</option>
      </select>
    </div>
    <textarea id="editor" placeholder=".chipsets:&#10;input a&#10;input b&#10;output s&#10;and gate&#10;&#10;.links:&#10;a:1 gate:1&#10;b:1 gate:2&#10;s:1 gate:3" spellcheck="false"></textarea>
    <div class="error-msg" id="errorMsg"></div>
  </div>
  <div class="svg-panel" id="svgPanel">
    <div class="empty">Generate a circuit to see the visualization</div>
  </div>
</div>
<div class="status">
  <span id="statusLeft">Ready</span>
</div>
<script>
const editor = document.getElementById('editor');
const svgPanel = document.getElementById('svgPanel');
const errorMsg = document.getElementById('errorMsg');
const info = document.getElementById('info');
const statusLeft = document.getElementById('statusLeft');

const EXAMPLES = {
  "and": ".chipsets:\ninput in_1\ninput in_2\noutput out\nand gate\n\n.links:\nin_1:1 gate:1\nin_2:1 gate:2\nout:1 gate:3",
  "or": ".chipsets:\ninput in_1\ninput in_2\noutput out\nor gate\n\n.links:\nin_1:1 gate:1\nin_2:1 gate:2\nout:1 gate:3",
  "not": ".chipsets:\ninput in\noutput out\nnot gate\n\n.links:\nin:1 gate:1\nout:1 gate:2",
  "xor": ".chipsets:\ninput in_1\ninput in_2\noutput out\nxor gate\n\n.links:\nin_1:1 gate:1\nin_2:1 gate:2\nout:1 gate:3",
  "and-or-not": ".chipsets:\ninput in_a\ninput in_b\ninput in_c\noutput out\nnot not1\nnot not2\nnot not3\nnot not4\nor or\nand and\n\n.links:\nin_a:1 not1:1\nnot1:2 not2:1\nnot2:2 or:1\nin_b:1 not3:1\nnot3:2 or:2\nor:3 not4:1\nnot4:2 and:1\nin_c:1 and:2\nout:1 and:3",
  "4008_adder": ".chipsets:\ninput in_a1\ninput in_a2\ninput in_a3\ninput in_a4\ninput in_b1\ninput in_b2\ninput in_b3\ninput in_b4\ninput in_c\noutput out_0\noutput out_1\noutput out_2\noutput out_3\noutput out_c\n4008 adder\n\n.links:\nin_a1:1 adder:7\nin_a2:1 adder:5\nin_a3:1 adder:3\nin_a4:1 adder:1\nin_b1:1 adder:6\nin_b2:1 adder:4\nin_b3:1 adder:2\nin_b4:1 adder:15\nin_c:1 adder:9\nout_0:1 adder:10\nout_1:1 adder:11\nout_2:1 adder:12\nout_3:1 adder:13\nout_c:1 adder:14",
  "4071_or": ".chipsets:\ninput in_01\ninput in_02\noutput out_03\noutput out_04\ninput in_05\ninput in_06\ninput in_08\ninput in_09\noutput out_10\noutput out_11\ninput in_12\ninput in_13\n4071 gate\n\n.links:\nin_01:1 gate:1\nin_02:1 gate:2\nout_03:1 gate:3\nout_04:1 gate:4\nin_05:1 gate:5\nin_06:1 gate:6\nin_08:1 gate:8\nin_09:1 gate:9\nout_10:1 gate:10\nout_11:1 gate:11\nin_12:1 gate:12\nin_13:1 gate:13",
  "input_output": ".chipsets:\ninput in\noutput out\n\n.links:\nin:1 out:1"
};

function loadExample(name) {
  if (!name) return;
  editor.value = EXAMPLES[name] || '';
  document.getElementById('exampleSelect').value = '';
  generate();
}

function loadFile(event) {
  const file = event.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = function(e) {
    editor.value = e.target.result;
    generate();
  };
  reader.readAsText(file);
  event.target.value = '';
}

let lastSvg = '';

function generate() {
  const text = editor.value.trim();
  if (!text) {
    svgPanel.innerHTML = '<div class="empty">Enter a circuit definition</div>';
    return;
  }
  errorMsg.style.display = 'none';
  statusLeft.textContent = 'Generating...';

  fetch('/generate', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({nts: text})
  })
  .then(r => r.json())
  .then(data => {
    if (data.error) {
      errorMsg.textContent = data.error;
      errorMsg.style.display = 'block';
      statusLeft.textContent = 'Error';
      return;
    }
    lastSvg = data.svg;
    svgPanel.innerHTML = data.svg;
    info.textContent = data.info || '';
    statusLeft.textContent = 'Generated';
  })
  .catch(err => {
    errorMsg.textContent = 'Request failed: ' + err.message;
    errorMsg.style.display = 'block';
    statusLeft.textContent = 'Error';
  });
}

function downloadSvg() {
  if (!lastSvg) return;
  const blob = new Blob([lastSvg], {type: 'image/svg+xml'});
  const a = document.createElement('a');
  a.href = URL.createObjectURL(blob);
  a.download = 'circuit.svg';
  a.click();
  URL.revokeObjectURL(a.href);
}

editor.addEventListener('keydown', function(e) {
  if (e.ctrlKey && e.key === 'Enter') {
    e.preventDefault();
    generate();
  }
  if (e.key === 'Tab') {
    e.preventDefault();
    const start = this.selectionStart;
    const end = this.selectionEnd;
    this.value = this.value.substring(0, start) + '  ' + this.value.substring(end);
    this.selectionStart = this.selectionEnd = start + 2;
  }
});
</script>
</body>
</html>"""


class Handler(http.server.BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        pass

    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.end_headers()
        self.wfile.write(HTML_PAGE.encode())

    def do_POST(self):
        if self.path != "/generate":
            self.send_response(404)
            self.end_headers()
            return

        length = int(self.headers.get("Content-Length", 0))
        if length > 1_000_000:
            self._json_response(400, {"error": "File too large"})
            return

        body = self.rfile.read(length)
        try:
            data = json.loads(body)
        except json.JSONDecodeError:
            self._json_response(400, {"error": "Invalid JSON"})
            return

        nts_text = data.get("nts", "")
        if not nts_text.strip():
            self._json_response(400, {"error": "Empty input"})
            return

        try:
            circuit = NTSCircuit.parse_text(nts_text)
            if not circuit.components:
                self._json_response(200, {"error": "No components found. Check your .chipsets: section."})
                return
            svg = generate_svg(circuit, "")
            n_comp = len(circuit.components)
            n_links = len(circuit.links)
            types = set(c.ctype for c in circuit.components.values())
            self._json_response(200, {
                "svg": svg,
                "info": f"{n_comp} components, {n_links} links — types: {', '.join(sorted(types))}"
            })
        except Exception as e:
            self._json_response(200, {"error": f"Parse error: {e}"})

    def _json_response(self, code, obj):
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps(obj).encode())


def main():
    port = int(sys.argv[1]) if len(sys.argv) > 1 else PORT
    server = http.server.HTTPServer(("0.0.0.0", port), Handler)
    print(f"NTS Visualizer running at http://0.0.0.0:{port}")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        server.server_close()


if __name__ == "__main__":
    main()
