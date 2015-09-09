#include "BookEDM.h"

JUNO_BOOK_EDM(JM::GenHeader, JM::GenEvent, 100, /Event/Gen);
JUNO_BOOK_EDM(JM::SimHeader, JM::SimEvent, 200, /Event/Sim);
JUNO_BOOK_EDM(JM::MuonSimHeader, JM::MuonSimEvent, 210, /Event/MuonSim);
JUNO_BOOK_EDM(JM::ElecHeader, JM::ElecEvent, 250, /Event/Elec);
JUNO_BOOK_EDM(JM::CalibHeader, JM::CalibEvent, 300, /Event/Calib);
JUNO_BOOK_EDM(JM::RecHeader, JM::RecEvent, 400, /Event/Rec);
JUNO_BOOK_EDM(JM::RecTrackHeader, JM::RecTrackEvent, 410, /Event/RecTrack);
JUNO_BOOK_EDM(JM::PhyHeader, JM::PhyEvent, 500, /Event/Phy);
JUNO_BOOK_EDM(JM::TestHeaderA, JM::ATestEventA&JM::ATestEventB, 600, /Event/TestA);
JUNO_BOOK_EDM(JM::TestHeaderB, JM::BTestEventA&JM::BTestEventB, 700, /Event/TestB);
