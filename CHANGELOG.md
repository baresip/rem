# librem Changelog

All notable changes to librem will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [v2.9.0] - 2022-11-01

## What's Changed
* cmake: add pre-release version handling by @sreimers in https://github.com/baresip/rem/pull/95
* README.md: Update build instructions for cmake by @robert-scheck in https://github.com/baresip/rem/pull/96
* aubuf: exclude non compatible CXX functions by @sreimers in https://github.com/baresip/rem/pull/97
* auframe: skip level calculation if format is RAW by @alfredh in https://github.com/baresip/rem/pull/99
* ci: use actions/checkout@v3 by @sreimers in https://github.com/baresip/rem/pull/101
* vidframe_draw_point: add NV12 and NV21 pixel format by @alfredh in https://github.com/baresip/rem/pull/102


**Full Changelog**: https://github.com/baresip/rem/compare/v2.8.0...v2.9.0


## [v2.8.0] - 2022-10-01

* cmake: bump min. version 3.10 by @sreimers in https://github.com/baresip/rem/pull/79
* auframe: auframe\_bytes\_to\_timestamp use uint64\_t by @cspiel1 in https://github.com/baresip/rem/pull/80
* ci: migrate from make to CMake by @alfredh in https://github.com/baresip/rem/pull/81
* cmake: install improvements by @sreimers in https://github.com/baresip/rem/pull/83
* cmake: add static and shared targets by @sreimers in https://github.com/baresip/rem/pull/84
* cmake: add win32 linklibs by @sreimers in https://github.com/baresip/rem/pull/85
* debian: use dh-cmake by @sreimers in https://github.com/baresip/rem/pull/86
* vid/frame: fix possbile overflow multiplication by @sreimers in https://github.com/baresip/rem/pull/87
* cmake: add pkgconfig (fixes #90) by @robert-scheck in https://github.com/baresip/rem/pull/91
* cmake: fix shared API soversion (aligned with make) by @robert-scheck in https://github.com/baresip/rem/pull/89

**Full Changelog**: https://github.com/baresip/rem/compare/v2.7.0...v2.8.0

---

## [v2.7.0] - 2022-09-01

* cmake: add FindRE and use re-config.cmake for definitions by @sreimers in https://github.com/baresip/rem/pull/76
* Fixed prefix in Debian librem.pc by @juha-h in https://github.com/baresip/rem/pull/77

**Full Changelog**: https://github.com/baresip/rem/compare/v2.6.0...v2.7.0

---

## [v2.6.0] - 2022-08-01

* Make possible to use CMake --target install for library installation by @widgetii in https://github.com/baresip/rem/pull/68
* thread: thrd_error fixes by @sreimers in https://github.com/baresip/rem/pull/71
* ci/misc: bump pr-dependency-action@v0.5 by @sreimers in https://github.com/baresip/rem/pull/72
* aubuf: the ajb mode should respect wish size by @cspiel1 in https://github.com/baresip/rem/pull/73
* make aubuf usage simpler by @cspiel1 in https://github.com/baresip/rem/pull/74
* aubuf: prevent faulty timestamps by @cspiel1 in https://github.com/baresip/rem/pull/75

**Full Changelog**: https://github.com/baresip/rem/compare/v2.5.0...v2.6.0

---

## [v2.5.0] - 2022-07-01

* ci/build: fix macOS openssl path by @sreimers in https://github.com/baresip/rem/pull/59
* vidmix: use C11 mutex by @alfredh in https://github.com/baresip/rem/pull/58
* aubuf: fix possible data race warning by @cspiel1 in https://github.com/baresip/rem/pull/61
* aubuf: C11 mutex by @alfredh in https://github.com/baresip/rem/pull/62
* ajb: C11 mutex by @alfredh in https://github.com/baresip/rem/pull/63
* aubuf: correct ajb reset on frame drop and on underruns by @cspiel1 in https://github.com/baresip/rem/pull/64
* aubuf: better support for different put/get ptime by @cspiel1 in https://github.com/baresip/rem/pull/65

---

## [v2.4.0] - 2022-06-01

* aubuf overruns on startup by @cspiel1 in https://github.com/baresip/rem/pull/44
* h265: move from rem to re by @alfredh in https://github.com/baresip/rem/pull/45
* aubuf: do not drop frames if max size was not set by @cspiel1 in https://github.com/baresip/rem/pull/47
* h264: move from rem to re by @alfredh in https://github.com/baresip/rem/pull/46
* vidmix win32 fixes by @sreimers in https://github.com/baresip/rem/pull/49
* aumix: use new libre thread api by @sreimers in https://github.com/baresip/rem/pull/48
* aubuf/ajb: fix possible null pointer deref by @sreimers in https://github.com/baresip/rem/pull/50
* Fade in on underrun and reset ajb by @cspiel1 in https://github.com/baresip/rem/pull/51
* aubuf: add null pointer checks by @cspiel1 in https://github.com/baresip/rem/pull/52
* auconv: add auconv_to_float() by @alfredh in https://github.com/baresip/rem/pull/53
* audio: add optional decoding buffer by @cspiel1 in https://github.com/baresip/rem/pull/54

**Full Changelog**: https://github.com/baresip/rem/compare/v2.3.0...v2.4.0

---
