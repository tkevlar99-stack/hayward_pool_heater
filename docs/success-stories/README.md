# Success Stories

This folder collects user-contributed installation reports for Hayward-compatible pool heater controllers. These stories are not formal compatibility guarantees. They are structured field notes that help other users compare hardware, wiring, ESPHome versions, and known limitations before trying their own setup.

## Stories

- [PAC PRIMA START MONO 80 PS with PC1001 controller by @ed-fr](pac-prima-start-mono-80-ps-pc1001-ed-fr.md)

## Contributing A Story

Start from [TEMPLATE.md](TEMPLATE.md). A useful story should be factual, reproducible, and clear about what was actually tested.

Every contributed story must include attribution:

- GitHub handle, forum handle, or another contributor name.
- Source link when the story came from an issue, discussion, pull request, forum post, or other public source.
- Date of the original report or the date the story was added.

Please include the hardware details that made the setup work:

- Heater model and controller board, including any mismatch between manuals and board markings.
- Connector or display board location used for 12V, 5V, NET/data, and GND.
- ESP board, power supply, voltage regulator, level shifter, and GPIO mapping.
- Home Assistant and ESPHome versions.
- What works, what does not work, and what has not been tested.
- Links to YAML, code, logs, photos, or diagrams when available.

## Guidance For Agents And Maintainers

When normalizing contributed content:

- Preserve contributor attribution and source links.
- Keep the contributor's meaning intact while using the shared structure.
- Separate observed facts from interpretation.
- Mark unverified claims as unverified.
- Keep safety caveats visible, especially for active heater control.
- Do not present ESPHome compile success as proof that hardware control is safe.
- Do not copy full user YAML, secrets, or large code attachments into this folder unless the contributor clearly intended that content to be included in the repository.
- Redact Wi-Fi names, passwords, API keys, tokens, precise addresses, GPS data, and private photos.
- Prefer linking to the original issue or attachment for bulky material.

## Candidate Stories To Revisit

The root README contains older compatibility references that are useful, but they do not all have enough detail to become normalized success stories yet.

Known candidates:

- Issue #5 includes a comment from @ricko07 reporting that a PC1001 setup worked after correcting the ESP pin mapping from `GPIO22` to the board's D5/GPIO5 mapping. This needs more hardware detail before it should become a full story.
- Issue #8 asks about an Aqua-Pro 1100E board and is a compatibility investigation, not a success story.
- Issue #10 reports trouble with a PC1000/Hayward EnergyLine Pro setup and is not a success story.
