//# tImageConcat.cc: This program tests the ImageConcat class
//# Copyright (C) 1996,1997,1999,2000,2001
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$


#include <casacore/casa/aips.h>

#include <casacore/images/Images/ImageBeamSet.h>

#include <casacore/casa/Containers/Record.h>

#include <casacore/casa/Quanta/QLogical.h>

#include <casacore/casa/iostream.h>

#include <casacore/casa/namespace.h>

int main() {
    try {
        {
            cout << "*** Test constructors, operator=" << endl;
            // empty beam set
            ImageBeamSet x;
            AlwaysAssert(x.empty(), AipsError);
            AlwaysAssert(x.size() == 0, AipsError);
            AlwaysAssert(x.nelements() == 0, AipsError);
            AlwaysAssert(!x.hasSingleBeam(), AipsError);
            AlwaysAssert(!x.hasMultiBeam(), AipsError);

            // A beam.
            GaussianBeam beam(
                    Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                    Quantity(40, "deg")
            );


            ImageBeamSet b(20, 4);
            AlwaysAssert(!b.hasSingleBeam(), AipsError);
            AlwaysAssert(b.hasMultiBeam(), AipsError);
            b.set(beam);
            AlwaysAssert(b.getBeam(2,2) == beam, AipsError);

            // check operator=
            ImageBeamSet c = b;
            AlwaysAssert(c.size() == 20*4, AipsError);
            AlwaysAssert(b == b, AipsError);
            AlwaysAssert(c == b, AipsError);

            // check copy constructor
            ImageBeamSet d(b);
            AlwaysAssert(d == b, AipsError);
            c = x;
            AlwaysAssert(c.empty(), AipsError);
            x = b;
            AlwaysAssert(x.size() == 20*4, AipsError);
            AlwaysAssert(c != b, AipsError);
            AlwaysAssert(x == b, AipsError);

            // check a single beam
            ImageBeamSet k(beam);
            AlwaysAssert (k.shape() == IPosition(2,1,1), AipsError);
            AlwaysAssert (k.getBeam(2,2) == beam, AipsError);   // valid for all

            ImageBeamSet y(IPosition(2, 1, 4));
            y.set(beam);
            AlwaysAssert (y(2,3) == beam, AipsError);

            // Check assignment a bit more.
            y = b;
            AlwaysAssert(y == b, AipsError);
            y = ImageBeamSet();
            AlwaysAssert(y.empty(), AipsError);
        }
        {
            cout << "*** test setBeam()" << endl;
            GaussianBeam beam0(Quantity(4, "arcsec"), Quantity(3, "arcsec"), Quantity(20, "deg"));
            ImageBeamSet x(3, 4, beam0);
            AlwaysAssert (x.nchan() == 3, AipsError);
            AlwaysAssert (x.nstokes() == 4, AipsError);

            GaussianBeam beam1(Quantity(5, "arcsec"), Quantity(4, "arcsec"), Quantity(20, "deg"));
            x.setBeam(1, 2, beam1);
            IPosition axisPath = IPosition::makeAxisPath(x.shape().size());
            ArrayPositionIterator iter(x.shape(), axisPath, False);
            while (! iter.pastEnd()) {
                const IPosition pos = iter.pos();
                GaussianBeam beam = x.getBeam(pos[0], pos[1]);
                if (pos == IPosition(2, 1,2)) {
                    AlwaysAssert(beam == beam1, AipsError);
                }
                else {
                    AlwaysAssert(beam == beam0, AipsError);
                }
                iter.next();
            }
            {
                cout << "*** test setBeams()" << endl;
                GaussianBeam beam0(Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                        Quantity(20, "deg"));
                GaussianBeam beam1(Quantity(8, "arcsec"), Quantity(6, "arcsec"),
                        Quantity(10, "deg"));
                GaussianBeam beam2(Quantity(5, "arcsec"), Quantity(4, "arcsec"),
                        Quantity(20, "deg"));
                ImageBeamSet x00;
                ImageBeamSet x34(3, 4, beam0); x34.setBeam(1,2,beam2);
                ImageBeamSet x14(1, 4, beam0); x14.setBeam(0,1,beam2);
                ImageBeamSet x31(3, 1, beam0); x31.setBeam(1,0,beam2);
                ImageBeamSet x11(1, 1, beam0);
                ImageBeamSet b;
                b.setBeams (x00.getBeams());
                AlwaysAssert (b==x00, AipsError);
                b.setBeams (x34.getBeams());
                AlwaysAssert (b==x34, AipsError);
                b.setBeams (x14.getBeams());
                { ImageBeamSet t(3,4,beam0);
                t.setBeam(0,1,beam2); t.setBeam(1,1,beam2); t.setBeam(2,1,beam2);
                AlwaysAssert (b==t, AipsError); }
                b.setBeams (x31.getBeams());
                { ImageBeamSet t(3,4,beam0);
                t.setBeam(1,0,beam2); t.setBeam(1,1,beam2); t.setBeam(1,2,beam2);
                t.setBeam(1,3,beam2);
                AlwaysAssert (b==t, AipsError); }
                b.setBeams (x11.getBeams());
                { ImageBeamSet t(3,4,beam0);
                AlwaysAssert (b==t, AipsError); }
                { ImageBeamSet y(x11);
                y.setBeams (x34.getBeams());
                AlwaysAssert (y==x34, AipsError); }
                { ImageBeamSet y(x11);
                y.setBeams (x31.getBeams());
                AlwaysAssert (y==x31, AipsError); }
                { ImageBeamSet y(x31);
                y.setBeams (x34.getBeams());
                AlwaysAssert (y==x34, AipsError); }
                { ImageBeamSet y(x31);
                y.setBeams (x14.getBeams());
                ImageBeamSet t(3,4,beam0);
                t.setBeam(0,1,beam2); t.setBeam(1,1,beam2); t.setBeam(2,1,beam2);
                AlwaysAssert (y==t, AipsError); }
                { ImageBeamSet y(x14);
                y.setBeams (x31.getBeams());
                ImageBeamSet t(3,4,beam0);
                t.setBeam(1,0,beam2); t.setBeam(1,1,beam2); t.setBeam(1,2,beam2);
                t.setBeam(1,3,beam2);
                AlwaysAssert (y==t, AipsError); }
            }


            {
                cout << "*** test getting max and min area beams" << endl;
                GaussianBeam init(
                    Quantity(4, "arcsec"), Quantity(2, "arcsec"),
                    Quantity(0, "deg")
                );
                ImageBeamSet x(3, 4, init);
                AlwaysAssert(x.getMaxAreaBeam() == init, AipsError);
                AlwaysAssert(x.getMinAreaBeam() == init, AipsError);
                GaussianBeam maxBeam(
                    Quantity(10, "arcsec"), Quantity(8, "arcsec"),
                    Quantity(0, "deg")
                );
                GaussianBeam minBeam(
                    Quantity(1, "arcsec"), Quantity(1, "arcsec"),
                    Quantity(0, "deg")
                );
                IPosition maxBeamPos(2, 2, 1);
                IPosition minBeamPos(2, 2, 3);
                x.setBeam(maxBeamPos[0], maxBeamPos[1], maxBeam);
                x.setBeam(minBeamPos[0], minBeamPos[1], minBeam);

                AlwaysAssert(x.getMaxAreaBeam() == maxBeam, AipsError);
                AlwaysAssert(x.getMinAreaBeam() == minBeam, AipsError);
                AlwaysAssert(x.getMaxAreaBeamPosition() == maxBeamPos, AipsError);
                AlwaysAssert(x.getMinAreaBeamPosition() == minBeamPos, AipsError);
            }
            {
                cout << "*** test setBeams()" << endl;
                GaussianBeam init(
                    Quantity(4, "arcsec"), Quantity(2, "arcsec"),
                    Quantity(0, "deg")
                );

                ImageBeamSet x(1, 5, init);
                GaussianBeam beam2(
                    Quantity(10, "arcsec"), Quantity(5, "arcsec"),
                    Quantity(70, "deg")
                );
                GaussianBeam beam3(
                    Quantity(11, "arcsec"), Quantity(5, "arcsec"),
                    Quantity(70, "deg")
                );
                Matrix<GaussianBeam> beams(1, 5, beam2);
                beams(0, 3) = beam3;
                x.setBeams(beams);
                AlwaysAssert(x.getBeams().shape() == IPosition(2, 1, 5), AipsError);
                AlwaysAssert(x.getMaxAreaBeam() == beam3, AipsError);
            }
        }
        {
            cout << "*** test setBeam()" << endl;
            GaussianBeam beam0(
                Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                Quantity(20, "deg")
            );
            ImageBeamSet x(3, 4, beam0);
            GaussianBeam beam1(
                Quantity(5, "arcsec"), Quantity(4, "arcsec"),
                Quantity(20, "deg")
            );
            x.setBeam(1, 2, beam1);
            IPosition axisPath = IPosition::makeAxisPath(x.shape().size());
            ArrayPositionIterator iter(x.shape(), axisPath, False);
            while (! iter.pastEnd()) {
                const IPosition pos = iter.pos();
                GaussianBeam beam = x(pos[0], pos[1]);
                if (pos == IPosition(2, 1, 2)) {
                    AlwaysAssert(beam == beam1, AipsError);
                }
                else {
                    AlwaysAssert(beam == beam0, AipsError);
                }
                iter.next();
            }
            {
                cout << "*** Test setBeam(), both chan and stokes < 0" << endl;
                GaussianBeam beam0(
                    Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                    Quantity(20, "deg")
                );
                ImageBeamSet x(3, 4, beam0);
                GaussianBeam beam1(
                    Quantity(5, "arcsec"), Quantity(4, "arcsec"),
                    Quantity(20, "deg")
                );
                x.setBeam(-1, -1, beam1);
                AlwaysAssert(x.getBeams().size() == 1, AipsError);
                AlwaysAssert(x.getBeam() == beam1, AipsError);
            }
            {
                cout << "*** Test setBeam(), chan  < 0 && stokes >= 0" << endl;
                GaussianBeam beam0(
                    Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                    Quantity(20, "deg")
                );
                ImageBeamSet x(3, 4, beam0);
                GaussianBeam beam1(
                    Quantity(5, "arcsec"), Quantity(4, "arcsec"),
                    Quantity(20, "deg")
                );
                x.setBeam(-1, 2, beam1);
                AlwaysAssert(x.getBeams().size() == 12, AipsError);
                IPosition axisPath = IPosition::makeAxisPath(x.shape().size());
                ArrayPositionIterator iter(x.shape(), axisPath, False);
                while (! iter.pastEnd()) {
                    const IPosition pos = iter.pos();
                    GaussianBeam beam = x(pos[0], pos[1]);
                    if (pos[1] == 2) {
                        AlwaysAssert(beam == beam1, AipsError);
                    }
                    else {
                        AlwaysAssert(beam == beam0, AipsError);
                    }
                    iter.next();
                }
            }
            {
                cout << "*** Test setBeam(), stokes  < 0 && chan >= 0" << endl;
                GaussianBeam beam0(
                    Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                    Quantity(20, "deg")
                );
                ImageBeamSet x(3, 4, beam0);
                GaussianBeam beam1(
                    Quantity(5, "arcsec"), Quantity(4, "arcsec"),
                    Quantity(20, "deg")
                );
                x.setBeam(2, -1, beam1);
                AlwaysAssert(x.getBeams().size() == 12, AipsError);
                IPosition axisPath = IPosition::makeAxisPath(x.shape().size());
                ArrayPositionIterator iter(x.shape(), axisPath, False);
                while (! iter.pastEnd()) {
                    const IPosition pos = iter.pos();
                    GaussianBeam beam = x(pos[0], pos[1]);
                    if (pos[0] == 2) {
                        AlwaysAssert(beam == beam1, AipsError);
                    }
                    else {
                        AlwaysAssert(beam == beam0, AipsError);
                    }
                    iter.next();
                }
            }
            {
                cout << "*** test setBeams()" << endl;
                GaussianBeam init(
                    Quantity(4, "arcsec"), Quantity(2, "arcsec"),
                    Quantity(0, "deg")
                );
                ImageBeamSet x(1, 5, init);
                Matrix<GaussianBeam> beams(1, 5);
                x.setBeams(beams);
            }
        }
        {
            cout << "*** Test get max, min, median for polarizations" << endl;
            ImageBeamSet beamSet;
            IPosition pos;
            AlwaysAssert(
                beamSet.getMaxAreaBeamForPol(pos, 1) == GaussianBeam::NULL_BEAM,
                AipsError
            );
            AlwaysAssert(pos == IPosition(2, 0, 0), AipsError);

            GaussianBeam beam0(
                Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                Quantity(20, "deg")
            );
            beamSet = ImageBeamSet(beam0);
            beamSet.getMaxAreaBeamForPol(pos, 1);
            AlwaysAssert(pos==IPosition(2,0,0), AipsError);

            beamSet = ImageBeamSet(3,4, beam0);
            IPosition gotPos;
            for (uInt i=0; i<4; i++) {
                GaussianBeam gotBeam = beamSet.getMaxAreaBeamForPol(gotPos, i);
                AlwaysAssert(gotBeam == beam0, AipsError);
                AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);
                gotBeam = beamSet.getMinAreaBeamForPol(gotPos, i);
                AlwaysAssert(gotBeam == beam0, AipsError);
                AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);
                gotBeam = beamSet.getMedianAreaBeamForPol(gotPos, i);
                AlwaysAssert(gotBeam == beam0, AipsError);
                AlwaysAssert(gotPos == IPosition(2, 1, i), AipsError);
            }
            GaussianBeam beam1(
                Quantity(5, "arcsec"), Quantity(3, "arcsec"),
                Quantity(20, "deg")
            );
            beamSet.setBeam(2, 1, beam1);
            GaussianBeam beam2(
                Quantity(3, "arcsec"), Quantity(2, "arcsec"),
                Quantity(20, "deg")
            );
             beamSet.setBeam(1, 1, beam2);
            for (uInt i=0; i<4; i++) {
                GaussianBeam gotBeam = beamSet.getMaxAreaBeamForPol(gotPos, i);
                if (i == 1) {
                    AlwaysAssert(gotBeam == beam1, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 2, 1), AipsError);
                }
                else {
                    AlwaysAssert(gotBeam == beam0, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);
                }
                gotBeam = beamSet.getMinAreaBeamForPol(gotPos, i);
                if (i == 1) {
                    AlwaysAssert(gotBeam == beam2, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 1, i), AipsError);
                }
                else {
                    AlwaysAssert(gotBeam == beam0, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);
                }
                gotBeam = beamSet.getMedianAreaBeamForPol(gotPos, i);
                AlwaysAssert(gotBeam == beam0, AipsError);
                if (i == 1) {
                    AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);

                }
                else {
                    AlwaysAssert(gotPos == IPosition(2, 1, i), AipsError);
                }
            }

            beamSet = ImageBeamSet(4, 4, beam0);
            for (uInt i=0; i<4; i++) {
                GaussianBeam gotBeam = beamSet.getMaxAreaBeamForPol(gotPos, i);
                AlwaysAssert(gotBeam == beam0, AipsError);
                AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);
                gotBeam = beamSet.getMinAreaBeamForPol(gotPos, i);
                AlwaysAssert(gotBeam == beam0, AipsError);
                AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);
                gotBeam = beamSet.getMedianAreaBeamForPol(gotPos, i);
                AlwaysAssert(gotBeam == beam0, AipsError);
                AlwaysAssert(gotPos == IPosition(2, 2, i), AipsError);
            }
            beamSet.setBeam(2, 1, beam1);
            beamSet.setBeam(1, 1, beam2);
            GaussianBeam beam3(
                Quantity(4.5, "arcsec"), Quantity(3, "arcsec"),
                Quantity(20, "deg")
            );
            beamSet.setBeam(0, 1, beam3);
            for (uInt i=0; i<4; i++) {
                GaussianBeam gotBeam = beamSet.getMaxAreaBeamForPol(gotPos, i);
                if (i == 1) {
                    AlwaysAssert(gotBeam == beam1, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 2, 1), AipsError);
                }
                else {
                    AlwaysAssert(gotBeam == beam0, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);
                }
                gotBeam = beamSet.getMinAreaBeamForPol(gotPos, i);
                if (i == 1) {
                    AlwaysAssert(gotBeam == beam2, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 1, i), AipsError);
                }
                else {
                    AlwaysAssert(gotBeam == beam0, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);
                }
                gotBeam = beamSet.getMedianAreaBeamForPol(gotPos, i);
                if (i == 1) {
                    AlwaysAssert(gotBeam == beam3, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 0, i), AipsError);
                }
                else {
                    AlwaysAssert(gotBeam == beam0, AipsError);
                    AlwaysAssert(gotPos == IPosition(2, 2, i), AipsError);
                }
            }
        }
        {
          cout << "*** test equivalent()" << endl;
          GaussianBeam beam(Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                            Quantity(40, "deg"));
          GaussianBeam beam2(Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                             Quantity(40, "deg"));
          GaussianBeam beam3(Quantity(5, "arcsec"), Quantity(3, "arcsec"),
                             Quantity(40, "deg"));
          {
            ImageBeamSet set1;
            ImageBeamSet set2;
            AlwaysAssert(set1.equivalent(set2), AipsError);
            AlwaysAssert(set2.equivalent(set1), AipsError);
          }
          {
            ImageBeamSet set1;
            ImageBeamSet set2(1,1,beam);
            AlwaysAssert(! set1.equivalent(set2), AipsError);
            AlwaysAssert(! set2.equivalent(set1), AipsError);
          }
          {
            ImageBeamSet set1(4,3,beam);
            ImageBeamSet set2(3,4,beam);
            AlwaysAssert(! set1.equivalent(set2), AipsError);
            AlwaysAssert(! set2.equivalent(set1), AipsError);
          }
          {
            ImageBeamSet set1(1,3,beam);
            ImageBeamSet set2(3,1,beam);
            AlwaysAssert(set1.equivalent(set2), AipsError);
            AlwaysAssert(set2.equivalent(set1), AipsError);
          }
          {
            ImageBeamSet set1(1,3,beam);
            ImageBeamSet set2(3,1,beam);
            AlwaysAssert(set1.equivalent(set2), AipsError);
            AlwaysAssert(set2.equivalent(set1), AipsError);
          }
          {
            ImageBeamSet set1(1,1,beam);
            ImageBeamSet set2(3,1,beam);
            AlwaysAssert(set1.equivalent(set2), AipsError);
            AlwaysAssert(set2.equivalent(set1), AipsError);
          }
          {
            ImageBeamSet set1(1,1,beam);
            ImageBeamSet set2(3,4,beam);
            AlwaysAssert(set1.equivalent(set2), AipsError);
            AlwaysAssert(set2.equivalent(set1), AipsError);
          }
          {
            ImageBeamSet set1(1,4,beam);
            ImageBeamSet set2(3,4,beam);
            AlwaysAssert(set1.equivalent(set2), AipsError);
            AlwaysAssert(set2.equivalent(set1), AipsError);
          }
          {
            ImageBeamSet set1(3,1,beam);
            ImageBeamSet set2(3,4,beam2);
            AlwaysAssert(set1.equivalent(set2), AipsError);
            AlwaysAssert(set2.equivalent(set1), AipsError);
            set2.setBeam (2,3,beam3);
            AlwaysAssert(! set1.equivalent(set2), AipsError);
            AlwaysAssert(! set2.equivalent(set1), AipsError);
          }
        }
        {
            cout << "*** test getSmallestMinorAxis" << endl;
            Matrix<GaussianBeam> beams(1, 4);
            GaussianBeam beam1(
                Quantity(4, "arcsec"), Quantity(2, "arcsec"),
                Quantity(0, "deg")
            );
            GaussianBeam beam2(
                Quantity(4, "arcsec"), Quantity(2, "arcsec"),
                Quantity(20, "deg")
            );
            GaussianBeam beam3(
                Quantity(4, "arcsec"), Quantity(2, "arcsec"),
                Quantity(40, "deg")
            );
            GaussianBeam beam4(
                Quantity(4, "arcsec"), Quantity(2, "arcsec"),
                Quantity(60, "deg")
            );
            beams(0, 0) = beam1;
            beams(0, 1) = beam2;
            beams(0, 2) = beam3;
            beams(0, 3) = beam4;
            // all equal
            ImageBeamSet beamSet(beams);
            AlwaysAssert(beamSet.getSmallestMinorAxisBeam() == beam1, AipsError);
            beam3.setMajorMinor(Quantity(4, "arcsec"), Quantity(1, "arcsec"));
            beams(0, 2) = beam3;
            beamSet = ImageBeamSet(beams);
            GaussianBeam got = beamSet.getSmallestMinorAxisBeam();
            AlwaysAssert(got == beam3, AipsError);
            beam3.setMajorMinor(Quantity(3, "arcsec"), Quantity(2, "arcsec"));
            beams(0, 2) = beam3;
            beamSet = ImageBeamSet(beams);
            got = beamSet.getSmallestMinorAxisBeam();
            AlwaysAssert(got == beam3, AipsError);

            cout << "*** test to/fromRecord()" << endl;
            Record yy = beamSet.toRecord();
            ImageBeamSet gotSet = ImageBeamSet::fromRecord(yy);
            AlwaysAssert(
                gotSet.nchan() == beamSet.nchan() && gotSet.nstokes() == beamSet.nstokes()
                && gotSet.equivalent(beamSet),
                AipsError
            );
        }
        {
            cout << "*** Test getMedianAreaBeam()" << endl;
            Matrix<GaussianBeam> beams(3, 4);
            uInt count = 1;
            Matrix<GaussianBeam>::iterator iter = beams.begin();
            Matrix<GaussianBeam>::iterator end = beams.end();
            Quantity radius;
            while (iter != end) {
                radius = Quantity(count, "arcsec");
                iter->setMajorMinor(radius, radius);
                ++iter;
                ++count;
            }
            radius = Quantity(6.5, "arcsec");
            beams(2,2) = GaussianBeam(radius, radius, Quantity(0, "deg"));
            ImageBeamSet bs(beams);
            AlwaysAssert(bs.getMedianAreaBeam() == beams(2, 2), AipsError);

            Matrix<GaussianBeam> beams2(1, 12);
            count = 1;
            iter = beams2.begin();
            end = beams2.end();
            while (iter != end) {
                radius = Quantity(count, "arcsec");
                iter->setMajorMinor(radius, radius);
                iter++;
                count++;
            }
            radius = Quantity(6.5, "arcsec");
            beams2(0,10) = GaussianBeam(radius, radius, Quantity(0, "deg"));
            ImageBeamSet bs2(beams2);
            AlwaysAssert(bs2.getMedianAreaBeam() == beams2(0, 10), AipsError);

            Matrix<GaussianBeam> beams3(12, 1);
            count = 1;
            iter = beams3.begin();
            end = beams3.end();
            while (iter != end) {
                radius = Quantity(count, "arcsec");
                iter->setMajorMinor(radius, radius);
                iter++;
                count++;
            }
            radius = Quantity(6.5, "arcsec");
            beams3(8, 0) = GaussianBeam(radius, radius, Quantity(0, "deg"));
            ImageBeamSet bs3(beams3);
            AlwaysAssert(bs3.getMedianAreaBeam() == beams3(8,0), AipsError);
        }
        {
            cout << "*** test rotate()" << endl;
            GaussianBeam beam(
                Quantity(4, "arcsec"), Quantity(3, "arcsec"),
                Quantity(40, "deg")
            );
            ImageBeamSet beamSet(beam);
            beamSet.rotate(Quantity(30, "deg"));
            AlwaysAssert(
                beamSet.getBeam().getPA(True) == Quantity(70, "deg"), AipsError
            );
            AlwaysAssert(
                beamSet.getMinAreaBeam().getPA(True) == Quantity(70, "deg"), AipsError
            );
            AlwaysAssert(
                beamSet.getMaxAreaBeam().getPA(True) == Quantity(70, "deg"), AipsError
            );
            Matrix<GaussianBeam> beams(2,2, beam);
            beams(1, 1).setPA(Quantity(90, "deg"));
            beamSet = ImageBeamSet(beams);
            beamSet.rotate(Quantity(50, "deg"));
            AlwaysAssert(
                beamSet(0, 0).getPA(True) == Quantity(90, "deg"), AipsError
            );
            AlwaysAssert(
                beamSet(0, 1).getPA(True) == Quantity(90, "deg"), AipsError
            );
            AlwaysAssert(
                beamSet(1, 0).getPA(True) == Quantity(90, "deg"), AipsError
            );
            AlwaysAssert(
                beamSet(1, 1).getPA(True) == Quantity(-40, "deg"), AipsError
            );
        }
        const Quantity five(5, "arcsec");
        const Quantity four(4, "arcsec");
        const Quantity two(2, "arcsec");
        {
            cout << "*** check replacing largest beam works when chan specified "
                << "and stokes negative" << endl;
            Matrix<GaussianBeam> mat(1, 2);
            mat[0][0] = GaussianBeam(five, five, five);
            mat[1][0] = GaussianBeam(four, four, four);
            ImageBeamSet beams(mat);
            auto maxbeam = beams.getMaxAreaBeam();
            AlwaysAssert(maxbeam.getMajor().getValue() == 5, AipsError);
            beams.setBeam(0, -1, GaussianBeam(four, four, four));
            maxbeam = beams.getMaxAreaBeam();
            AlwaysAssert(maxbeam.getMajor().getValue() == 4, AipsError);
        }
        {
            cout << "*** check replacing largest beam works when stokes specified "
                << "and chan negative" << endl;
            Matrix<GaussianBeam> mat(2, 1);
            mat[0][0] = GaussianBeam(five, five, five);
            mat[0][1] = GaussianBeam(four, four, four);
            ImageBeamSet beams(mat);
            auto maxbeam = beams.getMaxAreaBeam();
            AlwaysAssert(maxbeam.getMajor().getValue() == 5, AipsError);
            beams.setBeam(-1, 0, GaussianBeam(four, four, four));
            maxbeam = beams.getMaxAreaBeam();
            AlwaysAssert(maxbeam.getMajor().getValue() == 4, AipsError);
        }
        {
            cout << "*** test getBeamAreas" << endl;
            Matrix<GaussianBeam> mat(3, 2);
            mat[0][0] = GaussianBeam(five, five, five);
            mat[0][1] = GaussianBeam(four, four, four);
            mat[0][2] = GaussianBeam(two, two, two);
            mat[1][0] = GaussianBeam(two, two, four);
            mat[1][1] = GaussianBeam(four, two, two);
            mat[1][2] = GaussianBeam(four, two, four);
            ImageBeamSet beams(mat);
            const auto areas_as2 = beams.getAreas().getValue("arcsec2");
            for (uint i=0; i<3; ++i) {
                for (uint j=0; j<2; ++j) {
                    AlwaysAssert(
                        areas_as2(i, j) == beams.getBeam(i,j).getArea("arcsec2"),
                        AipsError
                    );
                }
            }
        }
        {
            cout << "*** test paramMatrices" << endl;
            Matrix<GaussianBeam> mat(3, 2);
            mat[0][0] = GaussianBeam(five, five, five);
            mat[0][1] = GaussianBeam(four, four, four);
            mat[0][2] = GaussianBeam(two, two, two);
            mat[1][0] = GaussianBeam(two, two, four);
            mat[1][1] = GaussianBeam(four, two, two);
            mat[1][2] = GaussianBeam(four, two, four);
            ImageBeamSet beams(mat);
            auto matrices = beams.paramMatrices();
            const auto majors = matrices["major"].getValue();
            const auto minors = matrices["minor"].getValue();
            const auto pas = matrices["pa"].getValue();
            const auto mUnit = matrices["major"].getUnit();
            AlwaysAssert(mUnit == matrices["minor"].getUnit(), AipsError);
            const auto paUnit = matrices["pa"].getUnit();
            for (uint i=0; i<3; ++i) {
                for (uint j=0; j<2; ++j) {
                    const auto beam = beams.getBeam(i, j);
                    AlwaysAssert(
                        majors(i, j) == beam.getMajor(mUnit), AipsError
                    );
                    AlwaysAssert(
                        minors(i, j) == beam.getMinor(mUnit), AipsError
                    );
                    AlwaysAssert(
                        pas(i, j) == beam.getPA(paUnit),  AipsError
                    );
                }
            }
        }
    }
    catch (const std::exception& x) {
        cout << x.what() << endl;
        cout << "FAIL" << endl;
        return 1;
    }
    cout << "OK" << endl;
    return 0;
}

