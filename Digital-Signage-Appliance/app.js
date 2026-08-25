const signage = document.querySelector(".signage");
const params = new URLSearchParams(window.location.search);
const staticMode = params.has("static");
const reducedMotion = window.matchMedia("(prefers-reduced-motion: reduce)").matches;

if (staticMode) {
  document.body.dataset.static = "true";
}

requestAnimationFrame(() => {
  requestAnimationFrame(() => signage.classList.add("is-ready"));
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

  window.setInterval(() => {
    signage.classList.add("attention");
    window.setTimeout(() => signage.classList.remove("attention"), 1900);
  }, 12000);
}
