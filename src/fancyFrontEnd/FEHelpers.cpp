#include <FEHelpers.h>
#include <numbers>
#include <cmath>

double deg2rad(double deg) {
	return deg * std::numbers::pi / 180;
}

// Compute 2d coordinates for given lat lon coordinates using equirectangular approximation
// originLat/Lon is the "center" so our vehicles current position
std::pair<float, float> latLonTo2d(double lat, double lon, double olat, double olon) {
	double originLatRad = deg2rad(olat);

	double dLat = lat - olat;
	double dLon = lon - olon;

	double x = deg2rad(dLon) * EARTH_RADIUS_METERS * std::cos(originLatRad);
	double y = deg2rad(dLat) * EARTH_RADIUS_METERS;

	return { static_cast<float>(x), static_cast<float>(-y) };
}

