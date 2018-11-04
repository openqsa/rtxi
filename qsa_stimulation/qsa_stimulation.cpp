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

/*
 * This is a template implementation file for a user module derived from
 * DefaultGUIModel with a custom GUI.
 */

#include "qsa_stimulation.h"

#include <main_window.h>

#include <algorithm>
#include <iostream>

extern "C"
Plugin::Object * createRTXIPlugin()
{
        return new QsaStimulation();
}

namespace
{
DefaultGUIModel::variable_t vars[] =
{
        {
                "Iqsa", "A", DefaultGUIModel::OUTPUT
        },
        {
                "Sync", "", DefaultGUIModel::OUTPUT
        },
        {
                "MinFrequency", "Hz",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
        },
        {
                "MaxFrequency", "Hz",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
        },
        {
                "SeedFrequencies", "",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER,
        },
        {
                "Duration", "s",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
        },
        {
                "Amplitude", "current",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
        },
        {
                "SeedPhase", "",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER,
        },
        {
                "RestLevel", "current",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
        },
        {
                "StepLevel", "current",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
        },
        {
                "StepDelay", "s",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
        },
        {
                "DropDelay", "s",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
        },
        {
                "TraceCount", "",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER,
        },
        {
                "TracePause", "s",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
        },
        {
                "TraceAlternance", "",
                DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER,
        }
};

std::size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

long cycle = 0;
}

QsaStimulation::QsaStimulation()
:
        DefaultGUIModel("QsaStimulation with Custom GUI", ::vars, ::num_vars)
{
        setWhatsThis(
                "<p><b>QsaStimulation:</b>"
                "<br>Signal generator for QSA stimulation.</p>");
        DefaultGUIModel::createGUI(vars, num_vars);
        customizeGUI();
        initParameters();

        update(INIT);
        refresh();
        QTimer::singleShot(0, this, SLOT(resizeMe()));

        applyTimer = new QTimer(this);
        QObject::connect(
                applyTimer,
                SIGNAL(timeout()),
                this,
                SLOT(onTimerApply()));
        applyTimer->start(100);
}

QsaStimulation::~QsaStimulation()
{
}

void QsaStimulation::execute()
{
        double t = cycle * period / 1000;
        double qsa_output;
        int qsa_sync;
        stimulation.evaluate(t - t0, qsa_output, qsa_sync);
        output(indexIqsa) = qsa_output;
        output(indexSync) = qsa_sync;
        cycle++;
}

void QsaStimulation::initParameters()
{
        Amplitude = 0.000002;
        Duration = 2.0;
        MinFrequency = 1.0;
        MaxFrequency = 5.0;
        SeedFrequencies = 0;
        SeedPhase = 0;
        RestLevel = -0.000008;
        StepLevel = -0.000004;
        StepDelay = 1.0;
        DropDelay = 1.0;
        TraceCount = 1;
        TracePause = 1.0;
        TraceAlternance = 1;
}

void QsaStimulation::doModify()
{
        // Set output indices
        auto assign = [&](const std::string & name, std::size_t & index)
        {
                for (std::size_t i = 0; i < 2; i++)
                {
                        if (getName(OUTPUT, i) == name)
                        {
                                index = i;
                                break;
                        } 
                }
        };
        assign("Iqsa", indexIqsa);
        assign("Sync", indexSync);

        // Get parameters
        period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
        Amplitude = getParameter("Amplitude").toDouble();
        Duration = getParameter("Duration").toDouble();
        MinFrequency = getParameter("MinFrequency").toDouble();
        MaxFrequency = getParameter("MaxFrequency").toDouble();
        SeedFrequencies = getParameter("SeedFrequencies").toInt();
        SeedPhase = getParameter("SeedPhase").toInt();
        RestLevel = getParameter("RestLevel").toDouble();
        StepLevel = getParameter("StepLevel").toDouble();
        StepDelay = getParameter("StepDelay").toDouble();
        DropDelay = getParameter("DropDelay").toDouble();
        TraceCount = getParameter("TraceCount").toInt();
        TracePause = getParameter("TracePause").toDouble();
        TraceAlternance = getParameter("TraceAlternance").toInt();

        // Build stimulation
        Qsa::StimulationBuilder stimulation_builder;
        stimulation_builder
                .set_dt(period / 1000)
                .set_amplitude(Amplitude)
                .set_duration(Duration)
                .set_min_frequency(MinFrequency)
                .set_max_frequency(MaxFrequency)
                .set_seed_frequencies(SeedFrequencies)
                .set_seed_phases(SeedPhase)
                .set_rest_level(RestLevel)
                .set_step_level(StepLevel)
                .set_step_delay(StepDelay)
                .set_drop_delay(DropDelay)
                .set_trace_count(TraceCount)
                .set_trace_pause(TracePause)
                .set_trace_alternance(TraceAlternance);
        stimulation = stimulation_builder.build();

        // Make text
        textEdit->setText(QString::fromStdString(stimulation.to_string()));

        // Plot
        doPlot(
                stimulation.frequencies().fundamentals(),
                stimulation.amplitudes());

        // Enable buttons
        copyButton->setEnabled(true);
        applyButton->setEnabled(true);
}

void QsaStimulation::doPlot(
        const std::vector<double> & x,
        const std::vector<double> & y)
{
        if (x.size() < 2 || x.size() != y.size())
                return;
        scene->setSceneRect(graphicsView->rect());
        scene->clear();
        auto min_x = *std::min_element(std::begin(x), std::end(x));
        auto max_x = *std::max_element(std::begin(x), std::end(x));
        auto min_y = *std::min_element(y.begin(), y.end());
        auto max_y = *std::max_element(y.begin(), y.end());
        for (std::size_t i = 0; i < x.size(); i++)
        {
                auto xi =
                        scene->sceneRect().left()
                        + 10
                        + (scene->sceneRect().width() - 20)
                        * (x[i] - min_x) 
                        / (max_x - min_x);
                scene->addLine(xi, 10, xi, scene->sceneRect().height() - 10);
        }
        graphicsView->fitInView(scene->sceneRect());
        scene->invalidate();
}

void QsaStimulation::update(DefaultGUIModel::update_flags_t flag)
{
        switch (flag)
        {
        case INIT:
        {
                period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
                setParameter("Amplitude", Amplitude);
                setParameter("Duration", Duration);
                setParameter("MinFrequency", MinFrequency);
                setParameter("MaxFrequency", MaxFrequency);
                setParameter("SeedFrequencies", SeedFrequencies);
                setParameter("SeedPhase", SeedPhase);
                setParameter("RestLevel", RestLevel);
                setParameter("StepLevel", StepLevel);
                setParameter("StepDelay", StepDelay);
                setParameter("DropDelay", DropDelay);
                setParameter("TraceCount", TraceCount);
                setParameter("TracePause", TracePause);
                setParameter("TraceAlternance", TraceAlternance);
                break;
        }

        case MODIFY:
        {
                doModify();
                break;
        }

        case UNPAUSE:
                break;

        case PAUSE:
                break;

        case PERIOD:
        {
                period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
                break;
        }

        default:
                break;
        }
}

void QsaStimulation::resizeEvent(QResizeEvent * event)
{
        DefaultGUIModel::resizeEvent(event);
        if (scene != nullptr)
                graphicsView->fitInView(scene->sceneRect());
}

void QsaStimulation::customizeGUI()
{
        // Copy button
        copyButton = new QPushButton("Copy stimulation");
        copyButton->setEnabled(false);
        QObject::connect(
                copyButton,
                SIGNAL(clicked()),
                this,
                SLOT(onClickCopyButton()));

        // Copy group
        auto copyGroup = new QGroupBox;
        auto copyLayout = new QHBoxLayout;
        copyGroup->setLayout(copyLayout);
        copyLayout->addWidget(copyButton);

        // Apply button
        applyButton = new QPushButton("Apply stimulation");
        applyButton->setEnabled(false);
        QObject::connect(
                applyButton,
                SIGNAL(clicked()),
                this,
                SLOT(onClickApplyButton()));

        // Apply group
        auto applyGroup = new QGroupBox;
        auto applyLayout = new QHBoxLayout;
        applyGroup->setLayout(applyLayout);
        applyLayout->addWidget(applyButton);

        // Text edit
        textEdit = new QTextEdit;
        textEdit->setReadOnly(true);
        textEdit->setMinimumHeight(20);

        // Graphics view
        graphicsView = new QGraphicsView;
        graphicsView->resize(graphicsView->width(), 20);
        scene = new QGraphicsScene;
        graphicsView->setScene(scene);
        graphicsView->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        // Display group
        auto displayGroup = new QGroupBox;
        auto displayLayout = new QVBoxLayout;
        displayGroup->setLayout(displayLayout);
        displayLayout->addWidget(textEdit);
        displayLayout->addWidget(graphicsView);

        // Custom layout
        auto customLayout = DefaultGUIModel::getLayout();
        customLayout->addWidget(copyGroup, 2, 1);
        customLayout->addWidget(applyGroup, 2, 0);
        customLayout->addWidget(displayGroup, 1, 1);
        setLayout(customLayout);
}

void QsaStimulation::onClickCopyButton()
{
        auto clipboard = QApplication::clipboard();
        auto encoding = Qsa::StimulationConverter::print(stimulation);
        clipboard->setText(QString::fromStdString(encoding));
}

void QsaStimulation::onClickApplyButton()
{
        t0 = cycle * period / 1000;
        stimulation.apply();
}

void QsaStimulation::onTimerApply()
{
        applyButton->setEnabled(!stimulation.is_applying());
}
