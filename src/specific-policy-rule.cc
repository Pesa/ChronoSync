/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Yingdi Yu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#include "specific-policy-rule.h"
#include <ndn-cpp/security/signature/signature-sha256-with-rsa.hpp>

using namespace ndn;
using namespace ndn::ptr_lib;
using namespace std;


SpecificPolicyRule::SpecificPolicyRule(shared_ptr<Regex> dataRegex,
				       shared_ptr<Regex> signerRegex)
  : PolicyRule(PolicyRule::IDENTITY_POLICY, true)
  , m_dataRegex(dataRegex)
  , m_signerRegex(signerRegex)
{}

SpecificPolicyRule::SpecificPolicyRule(const SpecificPolicyRule& rule)
  : PolicyRule(PolicyRule::IDENTITY_POLICY, true)
  , m_dataRegex(rule.m_dataRegex)
  , m_signerRegex(rule.m_signerRegex)
{}

bool 
SpecificPolicyRule::matchDataName(const Data& data)
{ return m_dataRegex->match(data.getName()); }

bool 
SpecificPolicyRule::matchSignerName(const Data& data)
{ 
  try{
    SignatureSha256WithRsa sig(data.getSignature());
    Name signerName = sig.getKeyLocator().getName ();
    return m_signerRegex->match(signerName); 
  }catch(SignatureSha256WithRsa::Error &e){
    return false;
  }catch(KeyLocator::Error &e){
    return false;
  }
}

bool
SpecificPolicyRule::satisfy(const Data & data)
{ return (matchDataName(data) && matchSignerName(data)) ? true : false ; }

bool
SpecificPolicyRule::satisfy(const Name & dataName, const Name & signerName)
{ return (m_dataRegex->match(dataName) && m_signerRegex->match(signerName)); }
