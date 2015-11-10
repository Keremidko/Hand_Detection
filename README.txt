Hand Detection application. 

First click on the skin on your hand with the left mouse button, to learn its color. 
Then you can see in the main frame the detection of the finger tips. 
Don't put your hand infront of your face, also the background needs to be different color from your hand.

Implementation:

- Learned gaussian parameters for the skin segmentation.
- Find the biggest contour -> the Hand
- Create convex HULL
- Get defect points
- Calculate cosine between Convex Hull points, and defect points.
- Based on cosine threshold - classify the point as finger tip, or not