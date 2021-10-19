#pragma once

#ifndef STORAGE_TYPES_H
#define STORAGE_TYPES_H

//------------------------------------------------------------------------------
//  Created     : 27.08.2021
//  Author      : Vadim Yarmushov ...
//  ver         : 0.0.1
//  Description : StorageTypes.h - here define types for libstorage
//
//------------------------------------------------------------------------------

namespace tmk::storage {

    // add new type for unknown type: UNKNOWN_TYPE_OBSERVABLEOBJECT 
    enum class ObservableType { BILLET, MANDREL, PIPE, PIPE_COOLED, UNKNOWN_TYPE_OBSERVABLEOBJECT };

} // end namespace tmk::storage

#endif // STORAGE_TYPES_H