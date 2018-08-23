/*===========================================================================================================
 *
 * HUL - Hurna Lib
 *
 * Copyright (c) Michael Jeulin-Lagarrigue
 *
 *  Licensed under the MIT License, you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://github.com/Hurna/Hurna-Lib/blob/master/LICENSE
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 *=========================================================================================================*/
#ifndef MODULE_SORT_PICKER_LOG_HXX
#define MODULE_SORT_PICKER_LOG_HXX

#include <Logger/vector.hxx>

// STD includes
#include <random>

namespace hul
{
  namespace picker
  {
  template <typename IT>
  class First
  {
    public:
      std::pair<IT, String> operator()(const IT& begin, const IT& end)
      {
        const String comment =  "Pick first item as Pivot within [" +
                                ToString(begin.GetIndex()) + ", " + ToString(end.GetIndex()) + "] : picker[" +
                                ToString(begin.GetIndex()) + "]{" + ToString(*begin) + "}";

        return make_pair(begin, comment);
      }
  };

  template <typename IT>
  class Last
  {
    public:
      std::pair<IT, std::string> operator()(const IT& begin, const IT& end)
      {
        const String comment =  "Pick last item as Pivot within [" +
                                ToString(begin.GetIndex()) + ", " + ToString(end.GetIndex()) + "] : picker[" +
                                ToString(begin.GetIndex()) + "]{" + ToString(*begin) + "}";

        return make_pair(end - 1, comment);
      }
  };

  template <typename IT>
  class Middle
  {
    public:
      std::pair<IT, std::string> operator()(const IT& begin, const IT& end)
      {
        const auto lenght = static_cast<const int>(std::distance(begin, end));

        const String comment =  "Pick middle item as Pivot within [" +
                                ToString(begin.GetIndex()) + ", " + ToString(end.GetIndex()) + "] : picker[" +
                                ToString(begin.GetIndex()) + "]{" + ToString(*begin) + "}";

        return make_pair(begin + lenght / 2, comment);
      }
  };

  template <typename IT>
  class ThreeMedian
  {
    public:
      std::pair<IT, String> operator() (const IT& begin, const IT& end)
      {
        // Get middle element
        const auto lenght = static_cast<const int>(std::distance(begin, end));
        const auto middleValue = *(begin + lenght / 2);
        auto pivotPick = IT(begin, "middle");
        auto lastIt = IT(end - 1, "end");

        // Compute median
        const int x = *begin - middleValue;
        const int y = middleValue - *lastIt;
        const int z = *begin - *lastIt;
        if (x * y > 0) pivotPick += lenght / 2;      // Middle
        else if (x * z > 0) pivotPick += lenght - 1; // End
        //else  pivotPick = 0;                       // Begin

        // Comment
        const String comment = String("Pick median (of three values) as Pivot within [") +
                               begin.String() + ", " + pivotPick.String() + ", " + lastIt.String() +
                               "] --> picker[" + ToString(pivotPick.GetIndex()) + "]{" +
                               ToString(*pivotPick) + "}";

        return make_pair(pivotPick, comment);
      }
  };

  template <typename IT>
  class Random
  {
    public:
      Random(const int seed = 130888) : random(seed) {}
      std::pair<IT, std::string> operator()(const IT& begin, const IT& end)
      {
        const auto lenght = static_cast<const int>(std::distance(begin, end));
        auto pivotPick = IT(begin + random() % lenght, "rand");

        // Comment
        const String comment = String("Pick random item as Pivot within [") +
                               begin.String() + ", " + pivotPick.String() + ", " + end.String() +
                               "] --> picker[" + ToString(pivotPick.GetIndex()) + "]{" +
                               ToString(*pivotPick) + "}";

        return make_pair(pivotPick, comment);
      }

    private:
      std::mt19937 random;
  };
  }
}

#endif // MODULE_SORT_PICKER_LOG_HXX
