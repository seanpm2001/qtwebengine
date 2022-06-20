// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef CONTENT_RENDERER_CLIENT_QT_H
#define CONTENT_RENDERER_CLIENT_QT_H

#include "qtwebenginecoreglobal_p.h"
#include "content/public/renderer/content_renderer_client.h"
#include "components/spellcheck/spellcheck_buildflags.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "services/service_manager/public/cpp/local_interface_provider.h"

#include <QScopedPointer>

namespace blink {
class WebPlugin;
struct WebPluginParams;
}

namespace chrome {
class WebRtcLoggingAgentImpl;
}

namespace error_page {
class Error;
}

namespace visitedlink {
class VisitedLinkReader;
}

namespace web_cache {
class WebCacheImpl;
}

#if QT_CONFIG(webengine_spellchecker)
class SpellCheck;
#endif

namespace QtWebEngineCore {

class UserResourceController;
class RenderConfiguration;
class ContentRendererClientQt
    : public content::ContentRendererClient
    , public service_manager::LocalInterfaceProvider
{
public:
    ContentRendererClientQt();
    ~ContentRendererClientQt();

    // content::ContentRendererClient:
    void RenderThreadStarted() override;
    void ExposeInterfacesToBrowser(mojo::BinderMap* binders) override;
    void RenderFrameCreated(content::RenderFrame *render_frame) override;

    void PrepareErrorPage(content::RenderFrame *render_frame,
                          const blink::WebURLError &error,
                          const std::string &http_method,
                          std::string *error_html) override;
    void PrepareErrorPageForHttpStatusError(content::RenderFrame *render_frame,
                                            const blink::WebURLError &error,
                                            const std::string &http_method,
                                            int http_status,
                                            std::string *error_html)  override;

    uint64_t VisitedLinkHash(const char *canonical_url, size_t length) override;
    bool IsLinkVisited(uint64_t linkHash) override;
    std::unique_ptr<blink::WebPrescientNetworking> CreatePrescientNetworking(content::RenderFrame *render_frame) override;
    void AddSupportedKeySystems(std::vector<std::unique_ptr<media::KeySystemProperties>> *key_systems) override;

    void RunScriptsAtDocumentStart(content::RenderFrame *render_frame) override;
    void RunScriptsAtDocumentEnd(content::RenderFrame *render_frame) override;
    void RunScriptsAtDocumentIdle(content::RenderFrame *render_frame) override;
    bool IsPluginHandledExternally(content::RenderFrame *embedder_frame,
                                   const blink::WebElement &plugin_element,
                                   const GURL &original_url,
                                   const std::string &original_mime_type) override;
    bool OverrideCreatePlugin(content::RenderFrame *render_frame,
                              const blink::WebPluginParams &params,
                              blink::WebPlugin **plugin) override;
    bool IsOriginIsolatedPepperPlugin(const base::FilePath& plugin_path) override;

    void WillSendRequest(blink::WebLocalFrame *frame,
                         ui::PageTransition transition_type,
                         const blink::WebURL &url,
                         const net::SiteForCookies &site_for_cookies,
                         const url::Origin *initiator_origin,
                         GURL *new_url) override;

#if QT_CONFIG(webengine_webrtc) && QT_CONFIG(webengine_extensions)
    chrome::WebRtcLoggingAgentImpl *GetWebRtcLoggingAgent();
#endif


private:
#if BUILDFLAG(ENABLE_SPELLCHECK)
    void InitSpellCheck();
#endif
    // service_manager::LocalInterfaceProvider:
    void GetInterface(const std::string &name, mojo::ScopedMessagePipeHandle request_handle) override;

    void GetNavigationErrorStringsInternal(content::RenderFrame *renderFrame, const std::string &httpMethod,
                                           const error_page::Error &error, std::string *errorHtml);

    QScopedPointer<RenderConfiguration> m_renderConfiguration;
    QScopedPointer<UserResourceController> m_userResourceController;
    QScopedPointer<visitedlink::VisitedLinkReader> m_visitedLinkReader;
    QScopedPointer<web_cache::WebCacheImpl> m_webCacheImpl;
#if QT_CONFIG(webengine_spellchecker)
    QScopedPointer<SpellCheck> m_spellCheck;
#endif
#if QT_CONFIG(webengine_webrtc) && QT_CONFIG(webengine_extensions)
    std::unique_ptr<chrome::WebRtcLoggingAgentImpl> m_webrtcLoggingAgentImpl;
#endif
};

} // namespace

#endif // CONTENT_RENDERER_CLIENT_QT_H
