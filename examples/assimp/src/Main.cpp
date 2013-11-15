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

#include "minko/Minko.hpp"
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Assimp", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = SceneManager::create(canvas->context());
	
	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::ASSIMPParser>("obj")
		->registerParser<file::ASSIMPParser>("dae")
		->queue("effect/Basic.effect")
		->queue("male02.obj");

	sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root = scene::Node::create("root")
			->addComponent(sceneManager);

		auto camera = scene::Node::create("camera")
			->addComponent(Renderer::create(0x7f7f7fff))
			->addComponent(Transform::create(
				Matrix4x4::create()->lookAt(Vector3::create(0.f, 1.f, 0.f), Vector3::create(0.f, 1.5f, 3.f))
			))
			->addComponent(PerspectiveCamera::create(
				(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)PI * 0.25f, .1f, 1000.f)
			);
		root->addChild(camera);

		auto model = assets->node("male02.obj")
			->addComponent(Transform::create(Matrix4x4::create()->appendScale(.01f)));

		root->addChild(model);
		
		auto resized = canvas->resized()->connect([&](Canvas::Ptr canvas, uint w, uint h)
		{
			camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
		});

		auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint time, uint deltaTime)
		{
			sceneManager->nextFrame();
		});

		canvas->run();
	});

	sceneManager->assets()->load();

	return 0;
}

