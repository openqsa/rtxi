/*
 * Copyright (C) 2011 Georgia Institute of Technology, University of Utah,
 * Weill Cornell Medical College
 * 
 * Copyright (C) 2018 OpenQSA. Quadratic Sinusoidal Analysis.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <default_gui_model.h>

#include "../qsa/qsa.h"

class QsaStimulation : public DefaultGUIModel
{
        Q_OBJECT

public:
        QsaStimulation();
        virtual ~QsaStimulation();

        void execute();
        void createGUI(DefaultGUIModel::variable_t *, int);
        void customizeGUI();

protected:
        virtual void update(DefaultGUIModel::update_flags_t);
        virtual void resizeEvent(QResizeEvent * event);

private:
        void initParameters();
        void doModify();
        void doPlot(
                const std::vector<double> & x,
                const std::vector<double> & y);

        double Amplitude;
        double Duration;
        double MinFrequency;
        double MaxFrequency;
        int SeedFrequencies;
        int SeedPhase;
        double RestLevel;
        double StepLevel;
        double StepDelay;
        double DropDelay;
        int TraceCount;
        double TracePause;
        int TraceAlternance;
        double period;
        Qsa::Stimulation stimulation;
        QPushButton * copyButton;
        QPushButton * applyButton;
        QTimer * applyTimer;
        QTextEdit * textEdit;
        QGraphicsView * graphicsView;
        QGraphicsScene * scene = nullptr;
        double t0;
        std::size_t indexIqsa;
        std::size_t indexSync;

private slots:
        void onClickCopyButton();
        void onClickApplyButton();
        void onTimerApply();
};
