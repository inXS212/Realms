#pragma once
#include "../../Base/Logging/ILogged.h"
#include "../../Base/Allocators/Allocator.h"

#include <memory>

#include "MeshRegister.h"

namespace rlms {
	class GraphicsManagerImpl;

	class GraphicsManager {
	private:
		static std::unique_ptr<GraphicsManagerImpl> instance;
	public:
		static std::shared_ptr<LoggerHandler> GetLogger ();

		static void Initialize (Allocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

		static void Load ();
		static void Draw ();
		static void Unload ();
	};
}

