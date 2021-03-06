
require "NBodyTesting"
require "persistence"

local arg = {...}

assert(#arg == 5, "Test driver expected 5 arguments got " .. #arg)

local nbodyBinary = arg[1]
local testDir = arg[2]
local testName = arg[3]
local histogramName = arg[4]
local testBodies = arg[5]

local nbodyFlags = getExtraNBodyFlags()
eprintf("NBODY_FLAGS = %s\n", nbodyFlags)

math.randomseed(os.time())

-- Pick one of the random seeds used in generating these tests
local testSeeds = { "670828913", "886885833", "715144259", "430281807", "543966758" }
local testSeed = testSeeds[math.random(1, #testSeeds)]
--local testSeed = testSeeds[1]


refResults = {
   ["model_1"] = {
      ["100"] = {
         ["670828913"] = 1921.399019920596629,
         ["886885833"] = 2507.259567413587774,
         ["715144259"] = 2277.483069844485726,
         ["430281807"] = 1921.399019920596629,
         ["543966758"] = 1880.892858295289443
      },

      ["1024"] = {
         ["670828913"] = 4745.453858370087801,
         ["886885833"] = 5024.409732087724478,
         ["715144259"] = 5068.041460645472398,
         ["430281807"] = 5205.379022531156807,
         ["543966758"] = 5205.379022531156807
      },

      ["10000"] = {
         ["670828913"] = 12988.972447398160512,
         ["886885833"] = 13810.623768933184692,
         ["715144259"] = 13233.105481851351215,
         ["430281807"] = 13211.225057524350632,
         ["543966758"] = 13196.675140474046202
      }
   },

   ["model_2"] = {
      ["100"] = {
         ["670828913"] = 1609.590270961241231,
         ["886885833"] = 1651.020703246448420,
         ["715144259"] = 1637.497574410253719,
         ["430281807"] = 1691.965879082630636,
         ["543966758"] = 1673.613780795002867
      },

      ["1024"] = {
         ["670828913"] = 2227.660833501358411,
         ["886885833"] = 2315.765891129145984,
         ["715144259"] = 2218.976540229048169,
         ["430281807"] = 2221.856526162277078,
         ["543966758"] = 2322.599989602272672
      },

      ["10000"] = {
         ["670828913"] = 5487.594694945784795,
         ["886885833"] = 5448.047330770828012,
         ["715144259"] = 5644.313735281130903,
         ["430281807"] = 5619.263388670427958,
         ["543966758"] = 5506.644390509201003
      }
   },

   ["model_3"] = {
      ["100"] = {
         ["670828913"] = 20831.672944609566912,
         ["886885833"] = 4158.342887342659196,
         ["715144259"] = 20831.672944609566912,
         ["430281807"] = 20831.672944609566912,
         ["543966758"] = 20831.672944609566912
      },

      ["1024"] = {
         ["670828913"] = 19683.357772942861629,
         ["886885833"] = 19683.357772942861629,
         ["715144259"] = 20831.672944609566912,
         ["430281807"] = 20831.672944609566912,
         ["543966758"] = 19683.357772942861629
      },

      ["10000"] = {
         ["670828913"] = 20609.774305598719366,
         ["886885833"] = 20609.774305598719366,
         ["715144259"] = 20542.412593231350911,
         ["430281807"] = 20559.213177104018541,
         ["543966758"] = 20508.890734055767098
      }
   },

   ["model_4"] = {
      ["100"] = {
         ["670828913"] = 1799.171185687286879,
         ["886885833"] = 2277.483069844485726,
         ["715144259"] = 2041.712748860029933,
         ["430281807"] = 2136.768119994320841,
         ["543966758"] = 1880.892858295289443
      },

      ["1024"] = {
         ["670828913"] = 4297.624655101215467,
         ["886885833"] = 4474.277553728013118,
         ["715144259"] = 4859.653989499103773,
         ["430281807"] = 4383.393439114558532,
         ["543966758"] = 4537.941083163500480,
      },

      ["10000"] = {
         ["670828913"] = 12419.933215978146109,
         ["886885833"] = 12671.581299442164891,
         ["715144259"] = 12698.613831201830180,
         ["430281807"] = 12426.438304314313427,
         ["543966758"] = 12278.377601992500786
      }
   },

   ["model_5"] = {
      ["100"] = {
         ["670828913"] = 1691.965879082630636,
         ["886885833"] = 1668.424687240968524,
         ["715144259"] = 1663.609956216128694,
         ["430281807"] = 1763.790356694394632,
         ["543966758"] = 1679.226460686934615
      },

      ["1024"] = {
         ["670828913"] = 2615.639889686557126,
         ["886885833"] = 2529.035884453685412,
         ["715144259"] = 2514.847022158757682,
         ["430281807"] = 2696.061382864623738,
         ["543966758"] = 2599.537453758675838
      },

      ["10000"] = {
         ["670828913"] = 6786.360854979313444,
         ["886885833"] = 6924.128383389537703,
         ["715144259"] = 6924.128383389537703,
         ["430281807"] = 6888.094090768488968,
         ["543966758"] = 7076.173795963540215
      }
   },

   ["model_5_bounds_test"] = {
      ["100"] = {
         ["670828913"] = 2293.461447105789375,
         ["886885833"] = 2293.461447105789375,
         ["715144259"] = 2293.461447105789375,
         ["430281807"] = 2293.461447105789375,
         ["543966758"] = 2293.461447105789375
      },

      ["1024"] = {
         ["670828913"] = 2396.140691411172156,
         ["886885833"] = 2394.399844541705988,
         ["715144259"] = 2394.399844541705988,
         ["430281807"] = 2396.140691411172156,
         ["543966758"] = 2397.015867822942710
      },

      ["10000"] = {
         ["670828913"] = 2397.704230413203732,
         ["886885833"] = 2396.709303100393299,
         ["715144259"] = 2397.268855037544199,
         ["430281807"] = 2396.336403714798962,
         ["543966758"] = 2397.579822419849734
      }
   },

   ["model_6"] = {
      ["100"] = {
         ["670828913"] = 1634.558964577640609,
         ["886885833"] = 1685.320812575451555,
         ["715144259"] = 1637.497574410253719,
         ["430281807"] = 1654.941550436299394,
         ["543966758"] = 1659.127593566565565
      },

      ["1024"] = {
         ["670828913"] = 2543.534948039212850,
         ["886885833"] = 2553.378789008792410,
         ["715144259"] = 2529.035884453685412,
         ["430281807"] = 2632.122996622112169,
         ["543966758"] = 2604.863505232569878
      },

      ["10000"] = {
         ["670828913"] = 6828.667758161484926,
         ["886885833"] = 6819.231784222966780,
         ["715144259"] = 6936.203733575175647,
         ["430281807"] = 7029.038900895182451,
         ["543966758"] = 6907.276711569538747
      }

   },

   ["model_7"] = {
      ["100"] = {
         ["670828913"] = 1595.006415300545314,
         ["886885833"] = 1590.790395684833129,
         ["715144259"] = 1596.471364148635075,
         ["430281807"] = 1589.439809858796707,
         ["543966758"] = 1592.167513703366239
      },

      ["1024"] = {
         ["670828913"] = 1770.259141947453145,
         ["886885833"] = 1782.083058905258213,
         ["715144259"] = 1775.485942747362515,
         ["430281807"] = 1780.757924737491749,
         ["543966758"] = 1757.385831791496457
      },

      ["10000"] = {
         ["670828913"] = 2549.086667816361569,
         ["886885833"] = 2501.008517222524461,
         ["715144259"] = 2549.086667816361569,
         ["430281807"] = 2523.813625617845901,
         ["543966758"] = 2535.672807081904921
      }
   },

   ["model_8"] = {
      ["100"] = {
         ["670828913"] = 1631.765800374539140,
         ["886885833"] = 1707.256275556696892,
         ["715144259"] = 1685.320812575451555,
         ["430281807"] = 1737.062791416462005,
         ["543966758"] = 1763.790356694394632
      },

      ["1024"] = {
         ["670828913"] = 2702.164770174296336,
         ["886885833"] = 2807.285497185637723,
         ["715144259"] = 2779.608089287843995,
         ["430281807"] = 2772.841683857429416,
         ["543966758"] = 3173.359457324755112 
      },

      ["10000"] = {
         ["670828913"] = 7705.264585929591703,
         ["886885833"] = 7592.566811067717936,
         ["715144259"] = 7595.352155978878727,
         ["430281807"] = 7832.356639221798105,
         ["543966758"] = 7891.244817987421811
      }
   },

   ["model_9"] = {
      ["100"] = {
         ["670828913"] = 4158.342887342659196,
         ["886885833"] = 20831.672944609566912,
         ["715144259"] = 4158.342887342659196,
         ["430281807"] = 4158.342887342659196,
         ["543966758"] = 4158.342887342659196
      },

      ["1024"] = {
         ["670828913"] = 19683.357772942861629,
         ["886885833"] = 18663.077934598011780,
         ["715144259"] = 17750.547963206401619,
         ["430281807"] = 18663.077934598011780,
         ["543966758"] = 18663.077934598011780
      },

      ["10000"] = {
         ["670828913"] = 20780.057434976853983,
         ["886885833"] = 20762.907076408828289,
         ["715144259"] = 20780.057434976853983,
         ["430281807"] = 20745.784013406995655,
         ["543966758"] = 20780.057434976853983
      }
   },

   ["model_ninkovic"] = {
      ["100"] = {
         ["670828913"] = 1685.320812575451555,
         ["886885833"] = 1699.244378582303852,
         ["715144259"] = 1685.320812575451555,
         ["430281807"] = 1663.609956216128694,
         ["543966758"] = 1668.424687240968524
      },

      ["1024"] = {
         ["670828913"] = 2672.147152755753268,
         ["886885833"] = 2604.863505232569878,
         ["715144259"] = 2589.007107165841262,
         ["430281807"] = 2649.001335002143605,
         ["543966758"] = 2654.717899643074816
      },

      ["10000"] = {
         ["670828913"] = 7041.395861769448857,
         ["886885833"] = 7422.988057711500005,
         ["715144259"] = 7254.115504742262601,
         ["430281807"] = 7151.596576777113114,
         ["543966758"] = 7133.887097437056582
      }
   },


   ["model_triaxial"] = {
      ["100"] = {
         ["670828913"] = 20831.672944609566912,
         ["886885833"] = 4158.342887342659196,
         ["715144259"] = 20831.672944609566912,
         ["430281807"] = 20831.672944609566912,
         ["543966758"] = 20831.672944609566912
      },

      ["1024"] = {
         ["670828913"] = 19683.357772942861629,
         ["886885833"] = 18663.077934598011780,
         ["715144259"] = 19683.357772942861629,
         ["430281807"] = 17750.547963206401619,
         ["543966758"] = 19683.357772942861629
      },

      ["10000"] = {
         ["670828913"] = 20592.893969723943883,
         ["886885833"] = 20643.615175820417790,
         ["715144259"] = 20525.638466568569129,
         ["430281807"] = 20458.805273815047258,
         ["543966758"] = 20660.575838517212105
      }
   }
}



function resultCloseEnough(a, b)
   return math.abs(a - b) < 1.0e-10
end

errFmtStr = [[
Result differs from expected:
   Expected = %20.15f  Actual = %20.15f  |Difference| = %20.15f
]]

function runCheckTest(testName, histogram, seed, nbody, ...)
   local fileResults, bodyResults
   local ret, result

   if not generatingResults then
      -- Check if the result exists first so we don't waste time on a useless test
      fileResults = assert(refResults[testName], "Didn't find result for test file")
      bodyResults = assert(fileResults[nbody], "Didn't find result with matching bodies")
      refResult = assert(bodyResults[seed], "Didn't find result with matching seed")
   end

   --eprintf("CHECKTEST - Before runFullTest\n")

   ret = runFullTest{
      nbodyBin  = nbodyBinary,
      testDir   = testDir,
      testName  = testName,
      histogram = histogram,
      seed      = seed,
      cached    = false,
      extraArgs = { nbody }
   }

   --eprintf(ret.."\n")
   --eprintf("CHECKTEST - Before findLikelihood\n")

   result = findLikelihood(ret, false)

   --eprintf("CHECKTEST - Before write(ret)\n")

   io.stdout:write(ret)

   if generatingResults then
      io.stderr:write(string.format("Test result: %d, %d, %s: %20.15f\n", nbody, seed, testName, result))
      return false
   end

   if result == nil then
      return true
   end

   --eprintf("CHECKTEST - Before notClose\n")

   local notClose = not resultCloseEnough(refResult, result)
   if notClose then
      io.stderr:write(string.format(errFmtStr, refResult, result, math.abs(result - refResult)))
   end

   return notClose
end

-- return true if passed
function testProbabilistic(resultFile, testName, histogram, nbody, iterations)
   local testTable, histTable, answer
   local resultTable = persisence.load(resultFile)
   assert(resultTable, "Failed to open result file " .. resultFile)

   testTable = assert(resultTable[testName], "Did not find result for test " .. testName)
   histTable = assert(testTable[nbody], "Did not find result for nbody " .. tostring(nbody))
   answer = assert(histTable[nbody], "Did not find result for histogram " .. histogram)

   local minAccepted = answer.mean - 3.0 * answer.stddev
   local maxAccepted = answer.mean + 3.0 * answer.stddev

   local result = 0.0
   local z = (result - answer.mean) / answer.stddev


   return true
end



function getResultName(testName)
   return string.format("%s__results.lua", testName)
end

if runCheckTest(testName, histogramName, testSeed, testBodies) then
   os.exit(1)
end


