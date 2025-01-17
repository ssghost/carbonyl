#include "carbonyl/src/browser/host_display_client.h"

#include <utility>

#include "components/viz/common/resources/resource_format.h"
#include "components/viz/common/resources/resource_sizes.h"
#include "mojo/public/cpp/system/platform_handle.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/src/core/SkDevice.h"
#include "ui/gfx/skia_util.h"

#if BUILDFLAG(IS_WIN)
#include "skia/ext/skia_utils_win.h"
#endif

#include "carbonyl/src/browser/bridge.h"

namespace carbonyl {

LayeredWindowUpdater::LayeredWindowUpdater(
    mojo::PendingReceiver<viz::mojom::LayeredWindowUpdater> receiver)
    : receiver_(this, std::move(receiver)) {}

LayeredWindowUpdater::~LayeredWindowUpdater() = default;

void LayeredWindowUpdater::OnAllocatedSharedMemory(
    const gfx::Size& pixel_size,
    base::UnsafeSharedMemoryRegion region) {
  if (region.IsValid())
    shm_mapping_ = region.Map();
}

void LayeredWindowUpdater::Draw(const gfx::Rect& damage_rect,
                                DrawCallback draw_callback) {
  Renderer::Main()->DrawBackground(
    shm_mapping_.GetMemoryAs<uint8_t>(),
    shm_mapping_.size(),
    damage_rect
  );

  std::move(draw_callback).Run();
}

HostDisplayClient::HostDisplayClient()
    : viz::HostDisplayClient(gfx::kNullAcceleratedWidget) {}
HostDisplayClient::~HostDisplayClient() = default;

void HostDisplayClient::CreateLayeredWindowUpdater(
    mojo::PendingReceiver<viz::mojom::LayeredWindowUpdater> receiver) {
  layered_window_updater_ =
      std::make_unique<LayeredWindowUpdater>(std::move(receiver));
}

#if BUILDFLAG(IS_MAC)
void HostDisplayClient::OnDisplayReceivedCALayerParams(
    const gfx::CALayerParams& ca_layer_params) {}
#endif

#if BUILDFLAG(IS_LINUX) && !BUILDFLAG(IS_CHROMEOS)
void HostDisplayClient::DidCompleteSwapWithNewSize(
    const gfx::Size& size) {}
#endif

}  // namespace carbonyl
