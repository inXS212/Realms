#pragma once

#include "../../CoreTypes.h"
#include "../../Base/RlmsException.h"
#include "../../Base/Allocators/FreeListAllocator.h"
#include "../../Base/Logging/ILogged.h"
#include "IMesh.h"

#include "MeshNameSanitizer.h"

#include <map>
#include <memory>

namespace rlms {
	class MeshRegister : public ILogged {
	public:
		//links a mesh to an ID
		std::map<IMODEL_TYPE_ID, IMesh*> m_register;
		//links an id to one or multiples names
		std::map<std::string, IMODEL_TYPE_ID> m_dict;

		std::unique_ptr<FreeListAllocator> m_model_Allocator;
		
		std::string getLogName () override {
			return "MeshRegister";
		};

		void start (Allocator* const& alloc, size_t mesh_pool_size, std::shared_ptr<Logger> funnel = nullptr);
		void stop ();

		//word register is reserved by cpp standards so i keep the uppercase
		template<class M> void Register (IMODEL_TYPE_ID type_id, std::string&& filename);
		template<class M> void Register (IMODEL_TYPE_ID type_id, std::string&& filename, std::string&& alias);
		void RegisterAlias (IMODEL_TYPE_ID type_id, std::string&& name);

		void imports ();
		void optimises ();
		void loads ();
		void unloads ();
		void free ();

		IMesh* get (IMODEL_TYPE_ID const& type_id);
		IMesh* get (std::string && alias);
	};

	template<class M> inline void MeshRegister::Register (IMODEL_TYPE_ID type_id, std::string&& filename) {
		logger->tag (LogTags::Info) << "Loading Mesh from file(" << filename << "), with id("<< type_id <<").\n";
		//invalid template call
		if (!std::is_base_of<IMesh, M> ()) {
			logger->tag (LogTags::Error) << "Invalid Mesh type!\n";
			throw std::exception ("Invalid Mesh type.");
		}

		//invalid filename
		if (filename.empty ()) { //to be modified
			logger->tag (LogTags::Error) << "Invalid Voxelite path!\n";
			throw std::exception ("Invalid Voxelite path.");
		}

		logger->tag (LogTags::Dev) << "Generating Alias from filename.\n";
		std::string alias = MeshNameSanitizer::GetAlias (filename);
		alias = MeshNameSanitizer::Sanitize (alias);
		
		//valid
		IMesh* m = allocator::allocateNew<M, std::string> (*m_model_Allocator.get (), std::move (filename));
		m_register.emplace (std::make_pair (type_id, m));
		m_dict.emplace (std::make_pair (alias, type_id));
	}

	template<class M> inline void MeshRegister::Register (IMODEL_TYPE_ID type_id, std::string&& filename, std::string&& alias) {
		logger->tag (LogTags::Info) << "Loading Mesh from file(" << filename << "), with id(" << type_id << ") and alias("<< alias <<").\n";
		//invalid template call
		if (!std::is_base_of<IMesh, M> ()) {
			logger->tag (LogTags::Error) << "Invalid Mesh type!\n";
			throw std::exception ("Invalid Mesh type.");
		}

		//invalid filename
		if (filename.empty ()) { //to be modified
			logger->tag (LogTags::Error) << "Invalid Voxelite path!\n";
			throw std::exception ("Invalid Voxelite path.");
		}

		alias = MeshNameSanitizer::Sanitize (alias);
		//valid
		IMesh* m = allocator::allocateNew<M, std::string> (*m_model_Allocator.get (), std::move (filename));
		m_register.emplace (std::make_pair (type_id, m));
		m_dict.emplace (std::make_pair (alias, type_id));
	}
}

