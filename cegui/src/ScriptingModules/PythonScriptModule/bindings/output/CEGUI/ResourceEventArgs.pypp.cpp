// This file has been generated by Py++.

#include "boost/python.hpp"
#include "python_CEGUI.h"
#include "ResourceEventArgs.pypp.hpp"

namespace bp = boost::python;

void register_ResourceEventArgs_class(){

    { //::CEGUI::ResourceEventArgs
        typedef bp::class_< CEGUI::ResourceEventArgs, bp::bases< CEGUI::EventArgs > > ResourceEventArgs_exposer_t;
        ResourceEventArgs_exposer_t ResourceEventArgs_exposer = ResourceEventArgs_exposer_t( "ResourceEventArgs", "! EventArgs based class that is used for notifications regarding resources.\n", bp::init< CEGUI::String const &, CEGUI::String const & >(( bp::arg("type"), bp::arg("name") )) );
        bp::scope ResourceEventArgs_scope( ResourceEventArgs_exposer );
        ResourceEventArgs_exposer.add_property( "resourceName"
                        , bp::make_getter( &CEGUI::ResourceEventArgs::resourceName
                                , bp::return_value_policy< bp::return_by_value >() )
                        , bp::make_setter( &CEGUI::ResourceEventArgs::resourceName )
                        , "! String identifying the resource type this notification is about.\n\
        ! String identifying the name of the resource this notification is about.\n" );
        ResourceEventArgs_exposer.add_property( "resourceType"
                        , bp::make_getter( &CEGUI::ResourceEventArgs::resourceType
                                , bp::return_value_policy< bp::return_by_value >() )
                        , bp::make_setter( &CEGUI::ResourceEventArgs::resourceType )
                        , "! String identifying the resource type this notification is about.\n" );
    }

}