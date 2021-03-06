/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if defined(CHROMIUM)
#pragma once

#include "minko/Common.hpp"
#include "minko/Signal.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/AbstractCanvas.hpp"
#include "include/cef_render_handler.h"

using namespace minko;

namespace chromium
{
	class ChromiumRenderHandler : public CefRenderHandler
	{
	public:
		ChromiumRenderHandler(std::shared_ptr<AbstractCanvas> canvas, std::shared_ptr<render::AbstractContext> context);
		~ChromiumRenderHandler();
		
		bool
		GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect);

		void
		OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor);

		void
		OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect &rect);

		void
		OnPopupShow(CefRefPtr<CefBrowser> browser, bool shown);

		void
		OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height);

		void
		SetResized(int width, int height);

		void
		uploadTexture();

		void
		visible(bool v)
		{
			_visible = v;
		}

	private:
		void
		clearPopup();
		
		void
		drawPopup();

		bool
		generateTexture();

		void
		canvasResized(std::shared_ptr<AbstractCanvas> canvas, uint w, uint h);

		void
		drawRect(unsigned char* source, int x, int y, int w, int h);

	private:
		int _lastW;
		int _lastH;
		int _texW;
		int _texH;

		uint _popupShown;
		uint _popupUpdated;
		int _popupX;
		int _popupY;
		int _popupW;
		int _popupH;

		std::shared_ptr<AbstractCanvas> _canvas;
		std::shared_ptr<render::AbstractContext> _context;
		Signal<std::shared_ptr<AbstractCanvas>, uint, uint>::Slot _canvasResizedSlot;

	public:
		std::shared_ptr<render::Texture> renderTexture;

		bool textureChanged;

	private:
		bool			_visible;
		unsigned char*	_popupBuffer;
		unsigned char*	_viewBuffer;

		IMPLEMENT_REFCOUNTING(ChromiumRenderHandler);
	};
}
#endif