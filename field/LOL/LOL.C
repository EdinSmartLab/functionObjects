/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2018 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "LOL.H"
#include "volFields.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(LOL, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        LOL,
        dictionary
    );
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //
// useless:
bool Foam::functionObjects::LOL::calc(){return true;}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::LOL::LOL
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    fieldExpression(name, runTime, dict, "OH"),
    OH_max(readScalar(dict.lookup("OH_max")))
{
    //setResultName(typeName, "OH");
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::functionObjects::LOL::~LOL()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

// useless:
bool Foam::functionObjects::LOL::clear(){return true;}


bool Foam::functionObjects::LOL::execute()
{
    Info<<"I'm in LOL::execute ---------------------"<<endl;

    scalar LOL = great;
    if (foundObject<volScalarField>(fieldName_))
    {
        const volScalarField& OH = lookupObject<volScalarField>(fieldName_);

        IOdictionary sprayCloudProperties
        (
            IOobject
            (
                "sprayCloudProperties",
                mesh_.time().constant(),
                mesh_,
                IOobject::MUST_READ_IF_MODIFIED,
                IOobject::NO_WRITE
            )
        );
        vector position = sprayCloudProperties.subDict("subModels")
            .subDict("injectionModels").subDict("model1").lookup("position");
        vector direction = sprayCloudProperties.subDict("subModels")
            .subDict("injectionModels").subDict("model1").lookup("direction");

        forAll (OH, cellI)
        {
            if (OH[cellI] >= 0.02*OH_max)
            {
                vector raw = position - mesh_.C()[cellI];
                LOL = mag(raw&direction);
                if (mag(raw&direction) < LOL)
                {
                    LOL = mag(raw&direction);
                }
            }
        }
        Info << "LOL === " << LOL << endl;
    }
    else
    {
        Info << "LOL === " << LOL << endl;
    }

    return true;
}

bool Foam::functionObjects::LOL::write()
{
    Info<<"I'm in LOL::write --------------------------"<<endl;
    return true;
}


// ************************************************************************* //