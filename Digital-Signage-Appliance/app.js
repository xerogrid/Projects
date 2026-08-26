const signage = document.querySelector(".signage");
const params = new URLSearchParams(window.location.search);
const staticMode = params.has("static");
const reducedMotion = window.matchMedia("(prefers-reduced-motion: reduce)").matches;
const LOOP_MS = 18000;
const ORIGIN_DELAY_MS = {
  west: 0,
  north: 160,
  east: 280,
  south: 400,
  branch: 1180,
};
const ORIGIN_STAGGER_MS = 90;

if (staticMode) {
  document.body.dataset.static = "true";
}

function pathEnd(path) {
  const length = path.getTotalLength();
  return path.getPointAtLength(length);
}

function setupTraceLengths() {
  const originCounts = {};
  const paths = [...document.querySelectorAll(".trace-lines path")];

  paths.forEach((path) => {
    const length = Math.ceil(path.getTotalLength());
    const origin = path.dataset.origin || "west";
    const index = originCounts[origin] || 0;
    originCounts[origin] = index + 1;
    const delay =
      (ORIGIN_DELAY_MS[origin] ?? 0) + index * ORIGIN_STAGGER_MS;

    path.style.setProperty("--len", String(length));
    path.style.setProperty("--delay", `${delay}ms`);
    path.dataset.delayMs = String(delay);
  });

  document.querySelectorAll(".trace-pads circle").forEach((pad) => {
    const x = Number(pad.getAttribute("cx"));
    const y = Number(pad.getAttribute("cy"));
    let nearestDelay = ORIGIN_DELAY_MS.branch;
    let nearestDistance = Infinity;

    paths.forEach((path) => {
      const end = pathEnd(path);
      const distance = (end.x - x) ** 2 + (end.y - y) ** 2;
      if (distance < nearestDistance) {
        nearestDistance = distance;
        nearestDelay = Number(path.dataset.delayMs);
      }
    });

    pad.style.setProperty("--delay", `${nearestDelay + 220}ms`);
  });
}

function cancelLoopAnimations(elements) {
  elements.forEach((element) => {
    element.getAnimations().forEach((animation) => animation.cancel());
  });
}

function loopKeyframes(delayMs, hidden, visible) {
  const wait = Math.min(delayMs / LOOP_MS, 0.18);
  return [
    { ...hidden, offset: 0 },
    { ...hidden, offset: wait },
    { ...visible, offset: wait + 0.22 },
    { ...visible, offset: 0.78 },
    { ...hidden, offset: 0.94 },
    { ...hidden, offset: 1 },
  ];
}

function showStill() {
  signage.classList.add("is-still");
}

function restartEtch() {
  const paths = [...document.querySelectorAll(".trace-lines path")];
  const pads = [...document.querySelectorAll(".trace-pads circle")];
  const timing = { duration: LOOP_MS, easing: "linear", fill: "both" };

  cancelLoopAnimations([...paths, ...pads]);
  signage.classList.add("is-etching");

  paths.forEach((path) => {
    const length = Number(path.style.getPropertyValue("--len"));
    const delay = Number(path.dataset.delayMs) || 0;
    path.animate(
      loopKeyframes(
        delay,
        { strokeDashoffset: length },
        { strokeDashoffset: 0 },
      ),
      timing,
    );
  });

  pads.forEach((pad) => {
    const delay = Number.parseFloat(pad.style.getPropertyValue("--delay")) || 0;
    pad.animate(loopKeyframes(delay, { opacity: 0 }, { opacity: 1 }), timing);
  });
}

setupTraceLengths();
signage.classList.add("is-traced");

requestAnimationFrame(() => {
  requestAnimationFrame(() => {
    signage.classList.add("is-ready");
    if (staticMode || reducedMotion) {
      showStill();
      return;
    }
    restartEtch();
    window.setInterval(restartEtch, LOOP_MS);
  });
});

if (!staticMode && !reducedMotion) {

  const driftPositions = [
    [0, 0],
    [2, -1],
    [-1, 2],
    [-2, -1],
    [1, 1],
  ];
  let driftIndex = 0;

  window.setInterval(() => {
    driftIndex = (driftIndex + 1) % driftPositions.length;
    const [x, y] = driftPositions[driftIndex];
    signage.style.setProperty("--drift-x", `${x}px`);
    signage.style.setProperty("--drift-y", `${y}px`);
  }, 30000);
}
