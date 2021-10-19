import dclprocessor
import storage

def WorkWithObject(obj, boost_path, obj_name):
    if (storage.framesetsCount(obj) > 0) :
        framesets = storage.framesets(obj)
        frame = storage.frame(framesets[i]);
        print("Frames number in the 1st framest of object: ", storage.framesCount(framesets[0])(
    }
    else {
        print("No framesets in the " + obj_name);
    }
    #developer corner with object in shared memory

dclprocessor.pipeline_middle_process(WorkWithObject)
