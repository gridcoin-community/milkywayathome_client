
function makeHistogram()
   return HistogramParams.create()
end

function makePotential()
   return nil
end

function makeContext()
   return NBodyCtx.create{
      timestep   = calculateTimestep(16, 0.2),
      timeEvolve = 4.0,
      eps2       = calculateEps2(4096, 0.2),
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
   return { }
end

