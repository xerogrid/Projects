const signage = document.querySelector(".signage");
const photoStage = document.querySelector(".photo-stage");
const photoImage = document.querySelector(".photo-stage img");
const photoCaption = document.querySelector(".photo-caption");
const params = new URLSearchParams(window.location.search);
const staticMode = params.has("static");
const reducedMotion = window.matchMedia("(prefers-reduced-motion: reduce)").matches;
const LOOP_MS = 18000;
const DEFAULT_PHOTO_HOLD_MS = 12000;
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

let mode = params.has("away") ? "away" : "booth";
let photos = [];
let photoHoldMs = DEFAULT_PHOTO_HOLD_MS;
let photoIndex = 0;
let cycleTimer = 0;

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

function hidePhoto() {
  signage.classList.remove("is-photo");
  if (photoStage) {
    photoStage.setAttribute("aria-hidden", "true");
  }
}

function showPhoto(photo) {
  if (!photoImage || !photo) {
    return;
  }
  photoImage.src = photo.src;
  photoImage.alt = photo.caption || "";
  if (photoCaption) {
    photoCaption.textContent = photo.caption || "";
  }
  signage.classList.add("is-photo");
  signage.classList.add("is-still");
  if (photoStage) {
    photoStage.removeAttribute("aria-hidden");
  }
}

function later(fn, ms) {
  window.clearTimeout(cycleTimer);
  cycleTimer = window.setTimeout(fn, ms);
}

function restartEtch() {
  const paths = [...document.querySelectorAll(".trace-lines path")];
  const pads = [...document.querySelectorAll(".trace-pads circle")];
  const timing = { duration: LOOP_MS, easing: "linear", fill: "both" };

  cancelLoopAnimations([...paths, ...pads]);
  signage.classList.remove("is-still");
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

function showNextPhoto() {
  if (mode === "away") {
    return;
  }
  if (photoIndex >= photos.length) {
    runBrandCycle();
    return;
  }
  showPhoto(photos[photoIndex]);
  photoIndex += 1;
  later(showNextPhoto, photoHoldMs);
}

function afterBrand() {
  if (mode === "away") {
    return;
  }
  if (!photos.length) {
    runBrandCycle();
    return;
  }
  photoIndex = 0;
  showNextPhoto();
}

function runBrandCycle() {
  hidePhoto();
  if (mode === "away" || staticMode || reducedMotion) {
    showStill();
    return;
  }
  restartEtch();
  later(afterBrand, LOOP_MS);
}

function applyMode(next) {
  const normalized = next === "away" ? "away" : "booth";
  mode = normalized;
  document.body.dataset.mode = normalized;
  if (normalized === "away") {
    window.clearTimeout(cycleTimer);
    hidePhoto();
    showStill();
    return;
  }
  if (!staticMode && !reducedMotion) {
    runBrandCycle();
  }
}

async function loadPhotos() {
  try {
    const response = await fetch("assets/photos/manifest.json", {
      cache: "no-store",
    });
    if (!response.ok) {
      return;
    }
    const manifest = await response.json();
    const hold = Number(manifest.holdMs);
    if (Number.isFinite(hold) && hold >= 3000) {
      photoHoldMs = hold;
    }
    photos = (manifest.photos || [])
      .filter((photo) => photo && photo.src)
      .map((photo) => ({
        src: photo.src.includes("/")
          ? photo.src
          : `assets/photos/${photo.src}`,
        caption: photo.caption || "",
      }));
  } catch {
    photos = [];
  }
}

async function pollMode() {
  try {
    const response = await fetch("/api/mode", { cache: "no-store" });
    if (!response.ok) {
      return;
    }
    const payload = await response.json();
    if (payload.mode && payload.mode !== mode) {
      applyMode(payload.mode);
    }
  } catch {
    // Local file preview and python -m http.server have no control API.
  }
}

setupTraceLengths();
signage.classList.add("is-traced");
applyMode(mode);

requestAnimationFrame(() => {
  requestAnimationFrame(async () => {
    signage.classList.add("is-ready");
    await loadPhotos();
    if (staticMode || reducedMotion) {
      showStill();
      return;
    }
    if (mode !== "away") {
      runBrandCycle();
    }
    window.setInterval(pollMode, 2000);
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
