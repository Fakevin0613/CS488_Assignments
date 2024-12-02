# Set environment variables
export ENABLE_STOCHASTIC_SAMPLING=0
export ENABLE_SOFT_SHADOW=0
export ENABLE_REFLECTION=1
export ENABLE_REFRACTION=0
export ENABLE_GLOSSY_REFLECTION=0
export ENABLE_GLOSSY_REFRACTION=0
export ENABLE_MOTION_BLUR=0

# Echo the values to verify
echo "stochastic sampling: $ENABLE_STOCHASTIC_SAMPLING"
echo "soft shadow: $ENABLE_SOFT_SHADOW"
echo "reflections: $ENABLE_REFLECTION"
echo "refractions: $ENABLE_REFRACTION"
echo "glossy reflection: $ENABLE_GLOSSY_REFLECTION"
echo "glossy refraction: $ENABLE_GLOSSY_REFRACTION"
echo "motion blur: $ENABLE_MOTION_BLUR"