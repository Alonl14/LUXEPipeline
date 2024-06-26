#include "ActsLUXEPipeline/Sequencer.hpp"
#include "ActsLUXEPipeline/LUXEGeometry.hpp"
#include "ActsLUXEPipeline/LUXEBinnedMagneticField.hpp"
#include "ActsLUXEPipeline/ConstantBoundedField.hpp"
#include "Acts/Utilities/Logger.hpp"
#include <filesystem>

using namespace Acts::UnitLiterals;

/// @brief Run the propagation through 
/// a uniform energy spectrum and record the
/// energy vs position histograms for each layer
int main() {
    Acts::Logging::Level logLevel = Acts::Logging::VERBOSE;

    const std::vector<std::pair<Acts::ActsScalar,Acts::ActsScalar>> MagneticFieldBounds =
        {std::make_pair(-1000_mm,1000_mm),
            std::make_pair(1450_mm,2650_mm),
            std::make_pair(-100_mm,100_mm)};

    // setup the sequencer first w/ config derived from options
    Sequencer::Config seqCfg;
    seqCfg.events = 10;
    seqCfg.numThreads = -1;
    Sequencer sequencer(seqCfg);

//    LUXEROOTReader::LUXEROOTSimDataReader::Config readerCfg
//        = LUXEROOTReader::defaultSimConfig();
//    readerCfg.dataCollection = "SourceLink";
//    std::string pathToDir = "/home/romanurmanov/lab/LUXE/acts_LUXE_tracking/ActsLUXEPipeline_dataInRootFormat/SignalNextTrial_e1gpc_10.0_1";
    // map (x,y,z) -> (x,y,z)
    auto transformPos = [&](const Acts::Vector3& pos) {
        for (int i=0;i<3;i++) {
            if (pos[i] < MagneticFieldBounds[i].first ||
                pos[i] > MagneticFieldBounds[i].second) {
                return Acts::Vector3{0,1400,0};
            }
        }
        return pos;
    };

    // map (Bx,By,Bz) -> (Bx,By,Bz)
    auto transformBField = [](const Acts::Vector3& field, const Acts::Vector3&) {
        return field;
    };

    LUXEMagneticField::vGridOptions gridOpt;
    gridOpt.xBins = {-1000,-1, 0.,200, 1000.};
    gridOpt.yBins = {1300,1400,1450,1451, 2050.,2649,2650.,2651};
    gridOpt.zBins = {-100,-99, 0.,1, 100.};


    // Build the LUXE detector
    std::string gdmlPath = "lxgeomdump_stave_positron.gdml";
    std::vector<std::string> names = {"OPPPSensitive"};
    Acts::GeometryContext gctx;
    Acts::MagneticFieldContext mctx;
    LUXEGeometry::GeometryOptions gOpt;
    double B_z = 0.95_T;

    Acts::Extent dipoleExtent;
    dipoleExtent.set(Acts::binX, -1000_mm, 1000_mm);
    dipoleExtent.set(Acts::binY, 1450_mm, 2650_mm);
    dipoleExtent.set(Acts::binZ, -100_mm, 100_mm);

    auto BField = LUXEMagneticField::buildBinnedBField(
        LUXEMagneticField::ConstantBoundedField(Acts::Vector3(0., 0., -B_z), dipoleExtent),
        transformPos, transformBField, gridOpt, mctx);

    // auto positronArmBpr = LUXEGeometry::makeBlueprintPositron(gdmlPath, names, gOpt);
    // auto detector = LUXEGeometry::buildLUXEDetector(std::move(positronArmBpr), gctx, gOpt);

//    for (const auto & entry : std::filesystem::directory_iterator(pathToDir)) {
//        std::string pathToFile = entry.path();
//        readerCfg.filePaths.push_back(pathToFile);
//    }

    // The events are not sorted in the directory
    // but we need to process them in order
//    std::sort(readerCfg.filePaths.begin(), readerCfg.filePaths.end(),
//        [] (const std::string& a, const std::string& b) {
//            std::size_t idxRootA = a.find_last_of('.');
//            std::size_t idxEventA = a.find_last_of('t', idxRootA);
//            std::string eventSubstrA = a.substr(idxEventA + 1, idxRootA - idxEventA);
//
//            std::size_t idxRootB = b.find_last_of('.');
//            std::size_t idxEventB = b.find_last_of('t', idxRootB);
//            std::string eventSubstrB = b.substr(idxEventB + 1, idxRootB - idxEventB);
//
//            return std::stoul(eventSubstrA) < std::stoul(eventSubstrB);
//        }
//    );
//
//    readerCfg.filePaths = std::vector<std::string>(
//        readerCfg.filePaths.begin(), readerCfg.filePaths.begin() + 72);
//
//    // readerCfg.filePaths = {"/home/romanurmanov/lab/LUXE/acts_LUXE_tracking/ActsLUXEPipeline_dataInRootFormat/SignalNextTrial_e1gpc_10.0_1/dataFile_Signal_e1gpc_10.0_EFieldV10p7p1pyN17Vpercm_Processed_Stave25_Event83.root"};
//
//    sequencer.addReader(
//        std::make_shared<LUXEROOTReader::LUXEROOTSimDataReader>(readerCfg, logLevel));
//
//    IdealSeeder::Config seederCfg;
//    // seederCfg.roadWidth = 200;
//    seederCfg.inputSourceLinks = "SourceLink";
//    sequencer.addAlgorithm(
//        std::make_shared<IdealSeeder>(seederCfg, logLevel));

    // Run all configured algorithms and return the appropriate status.
//    return sequencer.run();
    return 0;
}