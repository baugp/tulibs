/***************************************************************************
 *   Copyright (C) 2008 by Ralf Kaestner                                   *
 *   ralf.kaestner@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "project.h"

#include "string/string.h"

config_param_t config_project_default_params[] = {
  {CONFIG_PROJECT_PARAMETER_NAME,
    config_param_type_string,
    "",
    "",
    "The name of the project"},
  {CONFIG_PROJECT_PARAMETER_VERSION,
    config_param_type_string,
    "",
    "",
    "The version of the project"},
  {CONFIG_PROJECT_PARAMETER_AUTHORS,
    config_param_type_string,
    "",
    "",
    "The authors of the project"},
  {CONFIG_PROJECT_PARAMETER_CONTACT,
    config_param_type_string,
    "",
    "",
    "The e-mail contact of the project maintainer"},
  {CONFIG_PROJECT_PARAMETER_HOME,
    config_param_type_string,
    "",
    "",
    "The URL of the official project homepage"},
  {CONFIG_PROJECT_PARAMETER_LICENSE,
    config_param_type_string,
    "",
    "",
    "The project licensing information"},
};

const config_default_t config_project_default_options = {
  config_project_default_params,
  sizeof(config_project_default_params)/sizeof(config_param_t),
};

void config_project_init(config_project_t* project, const char* name,
    const char* version, const char* authors, const char* contact,
    const char* home, const char* license) {
  string_init_copy(&project->name, name);
  string_init_copy(&project->version, version);

  string_init_copy(&project->authors, authors);
  string_init_copy(&project->contact, contact);
  string_init_copy(&project->home, home);
  string_init_copy(&project->license, license);
}

void config_project_init_config(config_project_t* project, const config_t*
    config) {
  config_project_init(project,
    config_get_string(config, CONFIG_PROJECT_PARAMETER_NAME),
    config_get_string(config, CONFIG_PROJECT_PARAMETER_VERSION),
    config_get_string(config, CONFIG_PROJECT_PARAMETER_AUTHORS),
    config_get_string(config, CONFIG_PROJECT_PARAMETER_CONTACT),
    config_get_string(config, CONFIG_PROJECT_PARAMETER_HOME),
    config_get_string(config, CONFIG_PROJECT_PARAMETER_LICENSE));
}

void config_project_init_copy(config_project_t* project, const
    config_project_t* src_project) {
  config_project_init(project, src_project->name, src_project->version,
    src_project->authors, src_project->contact, src_project->home,
    src_project->license);
}

void config_project_destroy(config_project_t* project) {
  string_destroy(&project->name);
  string_destroy(&project->version);

  string_destroy(&project->authors);
  string_destroy(&project->contact);
  string_destroy(&project->home);
  string_destroy(&project->license);
}
