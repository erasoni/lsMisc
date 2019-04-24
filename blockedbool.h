//SceneExplorer
//Exploring video files by viewer thumbnails
//
//Copyright (C) 2018  Ambiesoft
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef BLOCKEDTRUE_H
#define BLOCKEDTRUE_H

namespace Ambiesoft {


class BlockedBool
{
    volatile bool* target_;
    bool exitValue_;
public:
    BlockedBool(volatile bool* target, bool initialValue, bool exitValue) :
        target_(target),
        exitValue_(exitValue)
    {
        *target_ = initialValue;
    }
    BlockedBool(volatile bool* target) :
        BlockedBool(target, true, false){}
    ~BlockedBool()
    {
        *target_ = exitValue_;
    }
};


} // namespace

#endif // BLOCKEDTRUE_H