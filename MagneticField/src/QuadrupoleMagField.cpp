#include "ActsLUXEPipeline/QuadrupoleMagField.hpp"
// #include "G4RotationMatrix.hh"

// namespace
// {
//    G4RotationMatrix IdentityMatrix; 
// }

QuadrupoleMagField::QuadrupoleMagField(Acts::ActsScalar gradient) 
    : m_gradient(gradient) {};

QuadrupoleMagField::QuadrupoleMagField(
    Acts::ActsScalar gradient, 
    const Acts::Vector3& origin, 
    const Acts::RotationMatrix3& rotation) :  
        m_gradient(gradient),
        m_origin(origin),
        m_rotation(rotation) {};

QuadrupoleMagField::~QuadrupoleMagField() = default;

/// @brief Get the magnetic field cache
///
/// @param mctx Magnetic field context
/// @return magnetic field cache
Acts::MagneticFieldProvider::Cache QuadrupoleMagField::makeCache(
    const Acts::MagneticFieldContext& mctx) const {
        return Acts::MagneticFieldProvider::Cache(
            std::in_place_type<Cache>, mctx);
}

Acts::Result<Acts::Vector3> QuadrupoleMagField::getField(
    const Acts::Vector3& position, 
    MagneticFieldProvider::Cache& cache) const {
        Acts::Vector3 global(
            position.x() - m_origin.x(),
            position.y() - m_origin.y(),
            position.z() - m_origin.z());
        
        const Acts::Vector3 local = m_rotation * global;
        const Acts::Vector3 localB( 
            m_gradient * local.y(),
            m_gradient * local.x(), 
            0);
        const Acts::Vector3 globalB = m_rotation.inverse() * localB;

        return Acts::Result<Acts::Vector3>::success(globalB);
}
