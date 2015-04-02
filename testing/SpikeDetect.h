/*
 Copyright (C) 2011 Georgia Institute of Technology

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <default_gui_model.h>

class SpikeDetect : public DefaultGUIModel
{

public:

  SpikeDetect(void);
  virtual
  ~SpikeDetect(void);

  // the main function runs every time step, contains model logic
  virtual void
  execute(void);

protected:

  // run each time model parameters are updated
  virtual void
  update(DefaultGUIModel::update_flags_t);

private:

  // parameters
  double thresh;
  double min_int;

  // time of last spike
  double last_spike;

  // the internal state variable, sent as output
  long long count;
  int state;
};
