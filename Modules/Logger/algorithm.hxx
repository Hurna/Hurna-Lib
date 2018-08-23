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
#ifndef MODULE_LOGGER_ALGORITHM_HXX
#define MODULE_LOGGER_ALGORITHM_HXX

#include <Logger/options.hxx>
#include <Logger/typedef.hxx>
#include <Logger/vector.hxx>

namespace HUL_Logger
{
  /// @class Algo_Traits
  ///
  template <typename Algo>
  class Algo_Traits
  {
    public:
      /// Instantiate a new json writer using the stream passed as
      /// argument and write algorithm information.
      ///
      /// @return stream reference filled up with Algo object information,
      ///         error information in case of failure.
      static Ostream& Build(Ostream& os, Options opts)
      {
        std::unique_ptr<Algo_Traits> algo = std::unique_ptr<Algo_Traits>(new Algo_Traits(os));
        algo->Write(opts);

        return os;
      }

      /// Use json writer passed as parameter to write Algo information.
      ///
      /// @return stream reference filled up with Algo object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer, Options opts)
      {
        Write(writer, opts);

        return writer;
      }

      /*static bool Build(Logger& logger)
      {
        logger.AddEntry("type", Algo::GetType());
        logger.AddEntry("version", Algo::GetVersion());
        logger.AddEntry("name", Algo::GetName());

        return true;
      }*/


    private:
      Algo_Traits(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                                 writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      Algo_Traits operator=(Algo_Traits&) {} // Not Implemented

      bool Write(Options opts) { return Write(*this->writer, opts); }

      static bool Write(Writer& writer, Options opts)
      {
        writer.Key("type");
        writer.String(GetType());
        writer.Key("version");
        writer.String("check how to handle versionning with visualizers / structures and releases.");
        //if (opts & OpGetName || opts & OpIsSub)
        //{
           writer.Key("name");
           writer.String(Algo::GetName());
        //}

        /*if (opts & OpGetDoc)
          Algo::WriteDoc(writer);
        if (opts & OpGetInfo)
          Algo::WriteInfo(writer);
        if (opts & OpGetSrc)
          Algo::WriteSrc(writer);*/

        return true;
      }

      static const String GetType() { return "algorithm"; }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

namespace hul
{
  /// @class Algo_Traits
  ///
  template <typename Algo>
  class Algo_Traits
  {
  public:
    static bool Build(Logger& logger)
    {
      logger.AddEntry("type", Algo::GetType());
      logger.AddEntry("version", Algo::GetVersion());
      logger.AddEntry("name", Algo::GetName());
      if (logger.GetCurrentLevel() > 0) logger.AddEntry("level", logger.GetCurrentLevel());

      return true;
    }
  };
}

#endif // MODULE_LOGGER_ALGORITHM_HXX
