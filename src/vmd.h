#ifndef VMD_H
#define VMD_H

#include <opencv2/opencv.hpp>
#include <string>

/*
 * Class providing a sequential implementation of a
 * video motion detection algorithm.
 */
class VMD {
public:
	/*
	 *	Default constructor.
	 */
	VMD() {};

	/*
	 *	Default destructor.
	 */
	~VMD() {};

	/*
	 * Run the video motion detection algorithm on the video
	 * located at the specified path.
	 */
    virtual void run(std::string videoPath);

	/*
	 * Run the video motion detection algorithm on the video
	 * located at the specified path a number of times equal to
	 * 'tries'. Time measurements are outputted to /benchmark/benchmark.cvs.
	 */
    virtual void benchmarkRun(std::string videoPath, int tries);
};
#endif 
