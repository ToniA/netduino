//---------------------------------------------------------------------------
//<copyright file="OneWireCommand.cs">
//
// Copyright 2010 Stanislav "CW" Simicek
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//</copyright>
//---------------------------------------------------------------------------
namespace CW.NETMF.Hardware
{
  public static class OneWireCommand
  {
    // ROM Function Commands

    /// <summary>
    /// The command used to read the unique identifier of a single slave device.
    /// </summary>
    public const byte ReadRom = 0x33;

    /// <summary>
    /// The command used for addressing when no specific slave is targeted.
    /// </summary>
    public const byte SkipRom = 0xCC;

    /// <summary>
    /// The command used to address individual slave device on the bus.
    /// </summary>
    /// <remarks>
    /// The command followed by a 64-bit unique identifier allows the bus master
    /// to address a specific slave device. Only the slave that exactly matches
    /// the 64-bit identifier will respond to the function command issued by
    /// the master; all other slaves on the bus will wait for a reset pulse.
    /// </remarks>
    public const byte MatchRom = 0x55;

    /// <summary>
    /// The command used to perform search operations on the bus.
    /// </summary>
    public const byte SearchRom = 0xF0;


    public const byte AlarmSearch = 0xEC;
  }
}
