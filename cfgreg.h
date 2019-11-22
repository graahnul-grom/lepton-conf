#ifndef LEPTON_CONF_CFG_REG_H_
#define LEPTON_CONF_CFG_REG_H_

/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2019 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include <liblepton/liblepton.h>




// struct represents a configuration key:
// NOTE: unique( grp, key )
//
struct _CfgEntry
{
    const gchar* grp_;
    const gchar* key_;
    const gchar* def_val_; // default value
    const gchar* desc_;
};

typedef struct _CfgEntry CfgEntry;



// private:
// find CfgEntry in global cfg registry by group name and key name
//
const CfgEntry*
cfgreg_lookup( const gchar* grp, const gchar* key );

// public:
// convenience func:
// find CfgEntry in global cfg registry by group name and key name,
//   and if found, return description for that entry
//
const gchar*
cfgreg_lookup_descr( const gchar* grp, const gchar* key );

const gchar*
cfgreg_lookup_dflt_val( const gchar* grp, const gchar* key );

// public:
//
gboolean
cfgreg_can_toggle( const gchar* val );

// public:
//
void
cfgreg_populate_ctx( EdaConfig* ctx );


#endif /* LEPTON_CONF_CFG_REG_H_ */

