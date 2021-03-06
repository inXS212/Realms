#include "MeshRegister.h"

#include "../../Base/RlmsException.h"

#include "MeshNameSanitizer.h"

void rlms::MeshRegister::start (Allocator* const& alloc, size_t mesh_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	m_model_Allocator = std::unique_ptr<FreeListAllocator> (new FreeListAllocator (alloc->allocate (mesh_pool_size), mesh_pool_size));

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::MeshRegister::stop () {
	logger->tag (LogTags::None) << "Stopping !" << '\n';

	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		allocator::deallocateDelete<IMesh> (*m_model_Allocator.get(), it->second);
	}

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

void rlms::MeshRegister::RegisterAlias (IMODEL_TYPE_ID type_id, std::string&& name) {
	name = MeshNameSanitizer::Sanitize (name);

	auto it = m_dict.find (name);
	if (it != m_dict.end()) {
		//alias already taken
		logger->tag (LogTags::Warning) << "Alias "<< name << " for id("<< type_id <<") already taken, by block of id(" << it->second << ")" << '\n';
		throw RlmsException ("Mesh Alias already Taken");
	}

	m_dict.emplace (std::make_pair (name, type_id));
}

void rlms::MeshRegister::imports () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->import ();
	}
}

void rlms::MeshRegister::optimises () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->optimise ();
	}
}

void rlms::MeshRegister::loads () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->load ();
	}
}

void rlms::MeshRegister::unloads () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->unload ();
	}
}

void rlms::MeshRegister::free () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		allocator::deallocateDelete<IMesh> (*m_model_Allocator.get (), it->second);
	}
}

IMesh* rlms::MeshRegister::get (IMODEL_TYPE_ID const& type_id) {
	return m_register[type_id];
}

IMesh* rlms::MeshRegister::get (std::string && alias) {
	auto it = m_dict.find (alias);
	if (it == m_dict.end ()) {
		throw RlmsException ("Mesh not found");
	}
	IMODEL_TYPE_ID const type_id = it->second;
	return m_register[type_id];
}
