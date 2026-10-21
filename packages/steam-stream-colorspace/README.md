# steam-stream-colorspace

LD_PRELOAD shim for Steam's ARM64 `streaming_client` (Remote Play, issue #285).

With V4L2 hardware decoding, `streaming_client` creates its NV12 textures with
`SDL.texture.create.colorspace` set to 0 (unknown). SDL3's Vulkan renderer
rejects that with `Unsupported YUV colorspace`, so Steam turns hardware
decoding off and resets the stream. The shim wraps
`SDL_CreateTextureWithProperties` and replaces that explicit 0 with BT.709
full range, which is what the host stream signals.

It only acts inside `streaming_client`; other processes pass straight through.
`launch-steam` preloads it for the Steam client tree, together with
`STEAMLINK_V4L2_RESET_AFTER_LAST_BUF=1` for the iris source-change loop.

Check `/tmp/streaming_client.log` for `CV4L2Accel` and
`Unsupported YUV colorspace` to confirm whether hardware decoding held.
