import sys
import unittest
import argparse
import sys
import os

# if FaceEngine is not installed in system
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to dir with FaceEngine*.so file - binding of luna-sdk")

args = parser.parse_args()

if len(sys.argv) == 1 or not args.bind_path or not os.path.isdir(args.bind_path):
    parser.print_help(sys.stderr)
    sys.exit(1)

path_to_binding = args.bind_path
print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

sys.path.append(path_to_binding)

import FaceEngine as f

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])


class TestFaceEngineRect(unittest.TestCase):

    def test_constructors(self):
        rect1 = f.Rect()
        rect0 = f.Rect()
        self.assertEqual(rect0, rect1)
        self.assertEqual(rect1.x, 0)
        self.assertEqual(rect1.y, 0)
        self.assertEqual(rect1.width, 0)
        self.assertEqual(rect1.height, 0)
        rect2 = f.Rect(2, 5, 7, 10)
        rect3 = f.Rect(f.Vector2i(0, 0), f.Vector2i(100, 120))
        self.assertEqual(rect3.x, 0)
        self.assertEqual(rect3.y, 0)
        self.assertEqual(rect3.width, 100)
        self.assertEqual(rect3.height, 120)
        self.assertNotEqual(rect2, rect3)
        self.assertTrue(rect0 == rect1)
        self.assertFalse(rect2 == rect3)

    def compare_rects(self, rect1, rect2):
        self.assertTrue(rect1.isValid())
        self.assertTrue(rect2.isValid())
        self.assertEqual(rect1.x, rect2.x)
        self.assertEqual(rect1.y, rect2.y)
        self.assertEqual(rect1.width, rect2.width)
        self.assertEqual(rect1.height, rect2.height)

    def test_methods(self):
        rect1 = f.Rect(0, 20, 30, 40)
        self.assertEqual(rect1.size().x, 30)
        self.assertEqual(rect1.size().y, 40)
        self.assertEqual(rect1.topLeft(), f.Vector2i(0, 20))
        self.assertEqual(rect1.center(), f.Vector2i(15, 40))
        self.assertEqual(rect1.bottomRight(), f.Vector2i(30, 60))
        self.assertEqual(rect1.top(), 20)
        self.assertEqual(rect1.bottom(), 60)
        self.assertEqual(rect1.left(), 0)
        self.assertEqual(rect1.right(), 30)
        self.assertEqual(rect1.right(), 30)
        rect2 = f.Rect()
        rect2.set(f.Vector2i(0, 20), f.Vector2i(30, 60))
        self.assertEqual(rect1, rect2)
        self.assertEqual(rect1, rect2)
        # rect2.adjust(0, 0, 100, 100)
        delta = 10
        rect_adjusted = rect1.adjusted(delta, delta, delta, delta)
        rect1.adjust(delta, delta, delta, delta)
        self.assertEqual(rect1, rect_adjusted)
        self.assertEqual(rect1.getArea(), rect1.width * rect1.height)
        self.assertTrue(rect1.inside(rect1), rect1.adjusted(-delta, -delta, -delta, -delta))
        self.assertTrue(rect1.isValid())
        not_valid_rect1 = f.Rect(0, 10, -30, 40)
        not_valid_rect2 = f.Rect(0, 10, 30, -40)
        self.assertFalse(not_valid_rect1.isValid())
        self.assertFalse(not_valid_rect2.isValid())
        rect2 = f.RectFloat(10, 11, 12, 13)
        rect1 = f.Rect(rect2)
        self.compare_rects(rect1, rect2)
        rect1 = f.Rect()
        rect1.set(rect2)
        self.compare_rects(rect1, rect2)


if __name__ == '__main__':
    unittest.main()