
arg = {...}

seed = argSeed
nbody = arg[1]

assert(seed ~= nil, "Seed argument not set for test unit")
assert(nbody ~= nil, "Number of bodies not set for test unit")

prng = DSFMT.create(seed)

dwarfMass = 16
dwarfRadius = 0.2

function makePotential()
   return Potential.create{
      spherical = Spherical.hernquist{ mass = 1.52954402E5, scale = 0.7 },
      disk      = Disk.miyamotoNagai{ mass = 4.45865888E5, scaleLength = 6.5, scaleHeight = 0.26 },
      disk2     = Disk.sech2Exponential{ mass = 9.0E3, scaleLength = 6.0, scaleHeight = 0.32 },
      halo      = Halo.nfw{ vhalo = 109, scaleLength = 22.25 }
   }
end

function makeContext()
   return NBodyCtx.create{
      timestep   = calculateTimestep(dwarfMass, dwarfRadius),
      timeEvolve = 3.945,
      eps2       = calculateEps2(nbody, dwarfRadius),
      criterion  = "sw93",
      useQuad    = true,
      theta      = 1.0,
      BestLikeStart = 0.95,
      BetaSigma     = 2.5,
      VelSigma      = 2.5,
      BetaCorrect   = 1.111,
      VelCorrect    = 1.111,
      IterMax       = 6
   }
end

function makeBodies(ctx, potential)
   local finalPosition, finalVelocity = reverseOrbit{
      potential = potential,
      position  = lbrToCartesian(ctx, Vector.create(218, 53.5, 28.7)),
      velocity  = Vector.create(-161, 85, 107),
      tstop     = 4.0,
      dt        = ctx.timestep / 10.0
   }

   return predefinedModels.plummer{
      nbody       = nbody,
      prng        = prng,
      position    = finalPosition,
      velocity    = finalVelocity,
      mass        = dwarfMass,
      scaleRadius = dwarfRadius,
      ignore      = false
   }
end

function makeHistogram()
   return HistogramParams.create{
     --Orphan Stream coordinate transformation angles
     phi = 128.79,
     theta = 54.39,
     psi = 90.70,
     
     -- ANGULAR RANGE AND NUMBER OF BINS
     lambdaStart = -150,
     lambdaEnd   = 150,
     lambdaBins  = 50,
     
     betaStart = -15,
     betaEnd   = 15,
     betaBins  = 1
}
end


