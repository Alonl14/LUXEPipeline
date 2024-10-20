#include "TrackingPipeline/Material/MaterialValidation.hpp"

#include "TrackingPipeline/Material/IMaterialWriter.hpp"

#include <stdexcept>

MaterialValidation::MaterialValidation(
    const MaterialValidation::Config& cfg,
    Acts::Logging::Level level)
        : IAlgorithm("MaterialValidation", level), m_cfg(cfg) {
            // Prepare the I/O collections
            m_outputMaterialTracks.initialize(m_cfg.outputMaterialTracks);
            // Check the configuration - material validater
            if (m_cfg.materialValidater == nullptr) {
                throw std::invalid_argument("Missing material validater.");
            }
            // Check the configuration - random number service
            if (m_cfg.randomNumberSvc == nullptr) {
                throw std::invalid_argument("Missing random number service.");
            }
}

ProcessCode MaterialValidation::execute(
    const AlgorithmContext& context) const {
        // Create a random number generator
        RandomEngine rng =
            m_cfg.randomNumberSvc->spawnGenerator(context);

        // Setup random number distributions for some quantities
        std::uniform_real_distribution<double> phiDist(
            m_cfg.phiRange.first,
            m_cfg.phiRange.second);
        std::uniform_real_distribution<double> etaDist(
            m_cfg.etaRange.first,
            m_cfg.etaRange.second);

        // The output recorded material track collection
        std::unordered_map<std::size_t, Acts::RecordedMaterialTrack>
            recordedMaterialTracks;
    
        // Loop over the number of tracks
        for (std::size_t iTrack = 0; iTrack < m_cfg.ntracks; ++iTrack) {
            // Generate a random phi and eta
            Acts::ActsScalar phi = phiDist(rng);
            Acts::ActsScalar eta = etaDist(rng);
            Acts::ActsScalar theta = 2 * std::atan(std::exp(-eta));
            Acts::Vector3 direction(
                std::cos(phi) * std::sin(theta),
                std::sin(phi) * std::sin(theta), std::cos(theta));
        
            // Record the material
            auto rMaterial = m_cfg.materialValidater->recordMaterial(
                context.geoContext, 
                context.magFieldContext, 
                m_cfg.startPosition->gen(rng),
                direction);
        
            recordedMaterialTracks.emplace_hint(
                recordedMaterialTracks.end(), 
                iTrack,
                rMaterial);
        }
        
        // Write the mapped and unmapped material tracks to the output
        m_outputMaterialTracks(context, std::move(recordedMaterialTracks));
        
        return ProcessCode::SUCCESS;
}
