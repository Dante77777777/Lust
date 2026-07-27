# Lust / Gluttony visual alignment QA

## Comparison target

- Source visual truth: `C:\Users\GCY\AppData\Local\Temp\codex-clipboard-8a79f7be-9b08-44cd-80b7-a79f909bc582.png`
- Source pixels: 1704 × 976
- Implementation: Lust Vision desktop application, default empty-camera state
- Intended implementation viewport: 1650 × 900 logical pixels at the system display density
- Iteration 1 implementation screenshot:
  `C:\Users\GCY\AppData\Local\Temp\codex-clipboard-e9c9d7da-8137-418e-9231-a879a98aa478.png`
- Iteration 1 implementation pixels: 1654 × 927
- Iteration 2 implementation screenshot:
  `C:\Users\GCY\AppData\Local\Temp\codex-clipboard-1ce6c0f5-e2d3-4ac1-ab87-708176b80f9a.png`
- Iteration 2 implementation pixels: 1654 × 932
- Combined full-view comparison:
  `C:\Users\GCY\AppData\Local\Temp\lust-gluttony-design-qa-comparison.png`
- Current implementation screenshot: pending after the image-information-bar fix
- Density normalization: both full-window captures were normalized to 800 pixels high for the combined comparison

## Evidence

- Full-view source reference opened and inspected.
- Existing Lust pre-change screenshot opened and inspected:
  `C:\Users\GCY\AppData\Local\Temp\codex-clipboard-c730454f-192a-4349-a953-2a653e39b689.png`
- Iteration 1 screenshot opened and compared with the source reference.
- Iteration 1 proved that the global QSS and main-window changes were present, but the three UI header changes had not entered the incremental build.
- Lust was then rebuilt with `--clean-first`; all three affected widget implementations were recompiled and the application launched successfully.
- Iteration 2 was placed beside the Gluttony reference in one normalized comparison image and reviewed at full-view scale.
- Focused review covered the left controls, central image footer, right parameter table, and bottom stream action.

## Findings

- [P1] Iteration 1 used stale child-widget objects
  - Location: camera controls, image panel, and parameter panel.
  - Evidence: the screenshot retained the old single-row buttons, omitted the image and panel headings, showed the old refresh layout, and still displayed `this is a tipTextBrowser`.
  - Impact: the intended layout hierarchy and most semantic control styles were absent.
  - Fix: completed a clean rebuild so `ControlWidget.cpp`, `viewwidget.cpp`, and `ParamWidget.cpp` were rebuilt against their changed UI headers.
- [P2] Image information bar does not span the canvas
  - Location: bottom edge of the real-time image panel.
  - Evidence: Gluttony spans the information strip across its image viewport, while Lust iteration 2 ends the strip before the right edge.
  - Impact: the preview footer looks clipped and weakens the otherwise consistent image-panel framing.
  - Fix: use the graphics view width instead of its height when sizing the information-bar overlay; implemented and rebuilt.
- [P1] Post-fix visual comparison is pending
  - Location: current Lust main window.
  - Evidence: iteration 2 exposed the footer-width issue; the corrected executable is running, but its post-fix screenshot is not yet available.
  - Impact: the P2 fix cannot be visually closed without post-fix evidence.
  - Fix: capture the currently open Lust window once more.

## Required fidelity surfaces

- Fonts and typography: Microsoft YaHei UI / Segoe UI, weights, hierarchy, and small UI text visibly align with Gluttony; no P0/P1/P2 mismatch found.
- Spacing and layout rhythm: three-column proportions, panel margins, control spacing, and radii align; the image-footer width was the only actionable mismatch and is fixed in code.
- Colors and visual tokens: background, white panels, blue primary actions, pale-blue selections, borders, and dark preview canvas visibly follow Gluttony.
- Image quality and asset fidelity: the checkerboard remains crisp and the magenta frame is virtual-camera content rather than a decorative asset; no substitute imagery was introduced.
- Copy and content: Chinese camera-operation labels are appropriate to Lust, the debug placeholder is removed, and parameter copy is functional content.

## Implemented alignment

- Applied the Gluttony blue-gray palette, typography, borders, radii, and dark image-canvas treatment.
- Reorganized camera actions into a readable two-column control grid.
- Added panel, section, image-preview, parameter, description, and stream-control hierarchy.
- Added a bottom status bar and responsive splitter size constraints.
- Replaced the debug placeholder in the parameter description area with an empty-state hint.

## Comparison history

- Iteration 1:
  - Earlier issue: Lust had default platform styling, cramped actions, weak section hierarchy, and an exposed debug placeholder.
  - Fixes made: global QSS resource, structured panels, revised spacing/proportions, semantic control styles, and refined empty state.
  - Post-fix visual evidence: the screenshot showed that only the global style, main window, status bar, and canvas changes were active; child-widget layout code was stale.
- Iteration 2:
  - Earlier issue: changed UI headers did not trigger the expected incremental recompilation of their consuming widget sources.
  - Fixes made: stopped the stale process and performed a clean rebuild of all 31 build steps; relaunched the resulting executable.
  - Post-fix visual evidence: the screenshot confirms the intended layout and style; it also exposed a P2 image-information-bar width defect.
- Iteration 3:
  - Earlier issue: the information overlay used `height()` as its width, so it stopped before the image panel's right edge.
  - Fixes made: both initial and resize geometry now use `width()`; the affected source was rebuilt and the application relaunched successfully.
  - Post-fix visual evidence: pending a screenshot of the currently open application.

## Implementation checklist

- Capture the current Lust window at its default 1650 × 900 viewport.
- Compare typography, spacing, colors, copy, and the image-canvas treatment against the Gluttony reference.
- Fix any visible P0/P1/P2 mismatch and repeat capture.

## Follow-up polish

- Consider adding the same icon family as Gluttony after the core visual comparison passes.

final result: blocked
