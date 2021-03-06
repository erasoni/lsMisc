//Copyright (C) 2017 Ambiesoft All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//1. Redistributions of source code must retain the above copyright
//notice, this list of conditions and the following disclaimer.
//2. Redistributions in binary form must reproduce the above copyright
//notice, this list of conditions and the following disclaimer in the
//documentation and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//SUCH DAMAGE.

#pragma once

namespace Ambiesoft { namespace stdosd {

template<class T>
class CBoolBase
{
    T value_;
public:
    CBoolBase() : value_(T()) {}
    operator T() const {
        return value_;
    }
	const CBoolBase<T>& operator=(const T& value) {
        value_=value;
        return *this;
    }
    T* operator&() {
        return &value_;
    }
    T toggle() {
        value_ = !value_;
        return value_;
    }
};

typedef CBoolBase<bool> Cbool;

#if defined(_WIN32) && defined(_WINDOWS_)
typedef CBoolBase<BOOL> CBool;
#endif

//class CBool {
//	BOOL b_;
//public:
//	CBool() : b_(FALSE) {}
//	operator BOOL() const { return b_; }
//	CBool& operator=(BOOL b) {
//		b_ = b;
//		return *this;
//	}
//	BOOL* operator&() {
//		return &b_;
//	}
//	BOOL toggle() {
//		b_ = !b_;
//		return b_;
//	}
//};


}}
