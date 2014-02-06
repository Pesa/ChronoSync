/* -*- Mode: C32++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2012 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#include "sync-socket.h"
#include "sync-logging.h"

using namespace std;
using namespace ndn;

INIT_LOGGER ("SyncSocket");

namespace Sync {

using ndn::shared_ptr;

SyncSocket::SyncSocket (const Name &syncPrefix, 
                        shared_ptr<Validator> validator,
                        shared_ptr<Face> face,
                        NewDataCallback dataCallback, 
                        RemoveCallback rmCallback )
  : m_newDataCallback(dataCallback)
  , m_validator(validator)
  , m_keyChain(new KeyChain())
  , m_face(face)
  , m_syncLogic (syncPrefix,
                 validator,
                 face,
                 bind(&SyncSocket::passCallback, this, _1),
                 rmCallback)
{}

SyncSocket::~SyncSocket()
{
}

bool 
SyncSocket::publishData(const Name &prefix, uint64_t session, const char *buf, size_t len, int freshness)
{
  uint64_t sequence = getNextSeq(prefix, session);
  
  Name dataName = prefix;
  dataName.append(boost::lexical_cast<string>(session)).append(boost::lexical_cast<string>(sequence));

  Data data(dataName);
  data.setContent(reinterpret_cast<const uint8_t*>(buf), len);
  data.setFreshnessPeriod(1000*freshness);

  m_keyChain->sign(data);
  
  m_face->put(data);
  
  SeqNo s(session, sequence + 1);
  m_sequenceLog[prefix] = s;
  m_syncLogic.addLocalNames (prefix, session, sequence);
  return true;
}

void 
SyncSocket::fetchData(const Name &prefix, const SeqNo &seq, const OnDataValidated& onValidated, int retry)
{
  Name interestName = prefix;
  interestName.append(boost::lexical_cast<string>(seq.getSession())).append(boost::lexical_cast<string>(seq.getSeq()));

  const OnDataValidationFailed& onValidationFailed = bind(&SyncSocket::onDataValidationFailed, this, _1);
  
  ndn::Interest interest(interestName);
  interest.setMustBeFresh(true);
  m_face->expressInterest(interest, 
                          bind(&SyncSocket::onData, this, _1, _2, onValidated, onValidationFailed), 
                          bind(&SyncSocket::onDataTimeout, this, _1, retry, onValidated, onValidationFailed));

}

void
SyncSocket::onData(const shared_ptr<const ndn::Interest>& interest, 
                   const shared_ptr<Data>& data,
                   const OnDataValidated& onValidated,
                   const OnDataValidationFailed& onValidationFailed)
{
  m_validator->validate(data, onValidated, onValidationFailed);
}

void
SyncSocket::onDataTimeout(const shared_ptr<const ndn::Interest>& interest, 
                          int retry,
                          const OnDataValidated& onValidated,
                          const OnDataValidationFailed& onValidationFailed)
{
  if(retry > 0)
    {
      m_face->expressInterest(*interest,
                              bind(&SyncSocket::onData,
                                   this,
                                   _1,
                                   _2,
                                   onValidated,
                                   onValidationFailed),
                              bind(&SyncSocket::onDataTimeout, 
                                   this,
                                   _1,
                                   retry - 1,
                                   onValidated,
                                   onValidationFailed));
                              
    }
  else
    _LOG_DEBUG("interest eventually time out!");
}

void
SyncSocket::onDataValidationFailed(const shared_ptr<const Data>& data)
{
  _LOG_DEBUG("data cannot be verified!");
}


uint64_t
SyncSocket::getNextSeq (const Name &prefix, uint64_t session)
{
  SequenceLog::iterator i = m_sequenceLog.find (prefix);

  if (i != m_sequenceLog.end ())
    {
      SeqNo s = i->second;
      if (s.getSession() == session)
        return s.getSeq();
    }
  return 0;
}

}//Sync
