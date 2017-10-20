/*---------------------------------------------------------------SafetyThird.cpp
 *
 * *********************** Don't put safety third !!!! *************************
 *
 * This is a reminder not to put safety third like your workplace does.
 * (after corporate profits and management whim)
 *
 * Author: Donald Delmar Davis, Suspect Devices
 * Liscence: "Simplified BSD License"
 *
 * Copyright (c) 2016, Donald Delmar Davis, Suspect Devices
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in thedocumentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------*/
/*
 * The safteyThird module is a place for items relating to the safety of machines.
 * Examples include:
 * Watchdog timer (a mechanism for keeping code from running away from its job)
 * K1, K2 ( in large machinery a pair of Kontactors designed to drop power )
 * E-stop ( A big red button or a lanyard for emergencies)
 * Light Curtain (a device to keep fingers from getting into dangerous areas)
 * red. (lights or other indicators that there may be problems.
 */
#include "SafetyThird.h"

SafetyThird safety;
