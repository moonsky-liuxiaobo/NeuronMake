/***************************************************************************
**
** Autohr : Yunpeng Song
** Contact: 413740951@qq.com
** Date:    2016-11-14
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef TOOLDEBUG_H
#define TOOLDEBUG_H
#include "core/dataframe.h"
#include "core/neuronengine.h"
#include "core/neuronenginefloat.h"
#include <time.h>
#include <iostream>

namespace  qri_neuron_lib {

class tooldebug{
  public:
    /*purpose: print the input dataframe
     *input:
     * @frame, the frame
     */
    void PrintDataframe(DataFrame &frame);
    /*purpose:Create a single row content randomly.
     *input:
     * @buffer, the buffer of a single row
     * @len, the length of the buffer, usually the same as the cols
     * @min, the min value of the generated value.
     * @max, the max value of the generated value.
     *return:
     * true if success.
     */
    bool FillRandomSingleRow(float * buffer, int len, float min, float max);
    /*purpose:Create a frame content randomly.
     *input:
     * @frame, the empty frame
     * @min, the min value of the generated value.
     * @max, the max value of the generated value.
     *return:
     * true if success.
     */
     bool FillRandomDataframe(DataFrame * frame,float min, float max);
     /*purpose:Save the data frame list
      *input:
      * @framelist, a list stores all frames
      * @path, file to save.
      *return:
      * true if success.
      */
      bool SaveDataFrameList(std::vector<DataFrame*> framelist,const char * path);
      /*purpose:Save the engine
       *input:
       * @engine, the neuron engine to save
       * @path, file to save.
       *return:
       * true if success.
       */
      void SaveEngine(NeuronEngine* engine,const char * path);
      void SaveEngineFloat(NeuronEngineFloat * engine,const char* path);
    };
}
#endif // DEBUGGINGTOOLKIT_H
