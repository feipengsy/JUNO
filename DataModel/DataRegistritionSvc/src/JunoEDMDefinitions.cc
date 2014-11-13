#include "DataRegistritionSvc/BookEDM.h"

JUNO_BOOK_EDM(JM::GenHeader, JM::GenEvent, 100, /Event/GenEvent);
JUNO_BOOK_EDM(JM::SimHeader, JM::SimEvent, 200, /Event/SimEvent);
JUNO_BOOK_EDM(JM::MuonSimHeader, JM::MuonSimEvent, 210, /Event/MuonSimEvent);
JUNO_BOOK_EDM(JM::ElecHeader, JM::ElecEvent, 250, /Event/ElecEvent);
JUNO_BOOK_EDM(JM::CalibHeader, JM::CalibEvent, 300, /Event/CalibEvent);
JUNO_BOOK_EDM(JM::RecHeader, JM::RecEvent, 400, /Event/RecEvent);
JUNO_BOOK_EDM(JM::RecTrackHeader, JM::RecTrackEvent, 410, /Event/RecTrackEvent);
JUNO_BOOK_EDM(JM::PhyHeader, JM::PhyEvent, 500, /Event/PhyEvent);
