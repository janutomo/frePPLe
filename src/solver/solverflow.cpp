/***************************************************************************
  file : $URL$
  version : $LastChangedRevision$  $LastChangedBy$
  date : $LastChangedDate$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * Copyright (C) 2007 by Johan De Taeye                                    *
 *                                                                         *
 * This library is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation; either version 2.1 of the License, or  *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public        *
 * License along with this library; if not, write to the Free Software     *
 * Foundation Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 *
 * USA                                                                     *
 *                                                                         *
 ***************************************************************************/

#define FREPPLE_CORE
#include "frepple/solver.h"

namespace frepple
{

bool sortFlow(const Flow* lhs, const Flow* rhs)
{
  return lhs->getPriority() < rhs->getPriority();
}


DECLARE_EXPORT void SolverMRP::solve(const Flow* fl, void* v)
{
  // Note: This method is only called for consuming flows and for the leading
  // flow of an alternate group. See SolverMRP::checkOperation

  SolverMRPdata* data = static_cast<SolverMRPdata*>(v);
  if (fl->hasAlternates())
  {
    // CASE I: It is an alternate flow.
    // We ask each alternate flow in order of priority till we find a flow
    // that has a non-zero reply.

    // 1) collect a list of alternates
    list<const Flow*> thealternates;
    const Flow *x = fl->hasAlternates() ? fl : fl->getAlternate();
    for (Operation::flowlist::const_iterator i = fl->getOperation()->getFlows().begin();
      i != fl->getOperation()->getFlows().end(); ++i)
      if ((i->getAlternate() == x || &*i == x)
        && i->getEffective().within(data->state->q_flowplan->getDate()))
        thealternates.push_front(&*i);

    // 2) Sort the list
    thealternates.sort(sortFlow);

    // 3) Loop through the alternates till we find a non-zero reply
    Date min_next_date(Date::infiniteFuture);
    FlowPlan *flplan = data->state->q_flowplan;
    for (list<const Flow*>::const_iterator i = thealternates.begin();
      i != thealternates.end();)
    {
      const Flow *curflow = *i;
      data->state->q_flowplan = flplan; // because q_flowplan can change

      // 3a) Switch to this flow
      if (data->state->q_flowplan->getFlow() != curflow)
        data->state->q_flowplan->setFlow(curflow);

      // 3b) Call the Python user exit if there is one
      if (userexit_flow)
      {
        PythonObject result = userexit_flow.call(data->state->q_flowplan);
        if (!result.getBool())
        {
          // Return value is false, alternate rejected
          if (data->getSolver()->getLogLevel()>1)
            logger << indent(curflow->getOperation()->getLevel())
              << "   User exit disallows consumption from '"
              << (*i)->getBuffer()->getName() << "'" << endl;
          // Move to the next alternate
          if (++i != thealternates.end() && data->getSolver()->getLogLevel()>1)
            logger << indent(curflow->getOperation()->getLevel()) << "   Alternate flow switches from '"
                  << curflow->getBuffer()->getName() << "' to '"
                  << (*i)->getBuffer()->getName() << "'" << endl;
          continue;
        }
      }

      // 3c) Ask the buffer
      data->state->q_qty = - data->state->q_flowplan->getQuantity();
      data->state->q_date = data->state->q_flowplan->getDate();
      Command* topcommand = data->getLastCommand();
      curflow->getBuffer()->solve(*this,data);

      // 3d) A positive reply: exit the loop
      if (data->state->a_qty > ROUNDING_ERROR) return;

      // 3e) Undo the plan on the alternate
      data->undo(topcommand);

      // 3f) Prepare for the next alternate
      if (data->state->a_date < min_next_date)
        min_next_date = data->state->a_date;
      if (++i != thealternates.end() && data->getSolver()->getLogLevel()>1)
        logger << indent(curflow->getOperation()->getLevel()) << "   Alternate flow switches from '"
              << curflow->getBuffer()->getName() << "' to '"
              << (*i)->getBuffer()->getName() << "'" << endl;
    }

    // 4) No alternate gave a good result
    data->state->a_date = min_next_date;
    data->state->a_qty = 0;
    if (data->getSolver()->getLogLevel()>1)
      logger << indent(fl->getOperation()->getLevel()) <<
        "   Alternate flow doesn't find supply on any alternate : "
        << data->state->a_qty << "  " << data->state->a_date << endl;
  }
  else
  {
    // CASE II: Not an alternate flow.
    // In this case, this method is passing control on to the buffer.
    data->state->q_qty = - data->state->q_flowplan->getQuantity();
    data->state->q_date = data->state->q_flowplan->getDate();
    if (data->state->q_qty != 0.0)
    {
      fl->getBuffer()->solve(*this,data);
      if (data->state->a_date > fl->getEffective().getEnd())
      {
        // The reply date must be less than the effectivity end date: after
        // that date the flow in question won't consume any material any more.
        if (data->getSolver()->getLogLevel()>1
          && data->state->a_qty < ROUNDING_ERROR)
          logger << indent(fl->getBuffer()->getLevel()) << "  Buffer '"
            << fl->getBuffer()->getName() << "' answer date is adjusted to "
            << fl->getEffective().getEnd()
            << " because of a date effective flow" << endl;
        data->state->a_date = fl->getEffective().getEnd();
      }
    }
    else
    {
      // It's a zero quantity flowplan.
      // E.g. because it is not effective.
      data->state->a_date = data->state->q_date;
      data->state->a_qty = 0.0;
    }
  }
}


}